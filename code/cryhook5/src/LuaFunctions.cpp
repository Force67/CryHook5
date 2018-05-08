
/*
 *  This source file is part of the Far Cry 5 ScriptHook by Force67
 *  More information regarding licensing can be found in LICENSE.md
 */

#include <Nomad/nomad_base_function.h>
#include <Utility/PathUtils.h>
#include <Hooking.h>

#include <LuaFunctions.h>
#include <Game.h>

#include <CryHook5.h>

static CScriptSystem* g_ScriptSystem;

// lua script system stuff
static int64_t*(*GetLuaState)();
static int32_t (*LuaSetFieldWrap)(int64_t*, int64_t, const char*);

static bool (*CScriptSystem_ExecuteLuaContext)(CScriptSystem*, const char*, bool);
static bool (*CScriptSystem_ExecuteLuaFile)(CScriptSystem*, const char*, size_t, const char*, bool);

// lua
static void (*lua_pushclosure)(int64_t*, Lua::lua_CFunction, int32_t);
static int64_t (*lua_pcall)(int64_t*, int32_t, int32_t, unsigned int);
static int32_t (*lua_loadbuffer)(int64_t*, const char*, size_t, const char*);
static int32_t (*lua_loadfile_ex)(int64_t*, const char*, const char*);
static int32_t (*lua_gettop)(int64_t*);
static const char* (*lua_tolstring)(int64_t*, int64_t, int64_t);

// usability impl
bool Lua::ExecuteLua(const char* code)
{
    return CScriptSystem_ExecuteLuaContext(g_ScriptSystem, code, false);
}

void Lua::RegisterFunction(const char* name, lua_CFunction fn)
{
    auto state = GetLuaState();

    lua_pushclosure(state, fn, 0);
    LuaSetFieldWrap(state, 0xFFFFFFFE, name);
}

bool Lua::RunFile(const wchar_t* path)
{
    auto state = GetLuaState();

    FILE* fh = nullptr;
    _wfopen_s(&fh, path, L"rb");

    if (!fh) return false;

    uint32_t magic = 0;
    fread(&magic, 4, 1, fh);

    // is it a compiled file
    if (magic == 0x1b4c7561)
    {
        // if not, reopen in text mode
        fclose(fh);
        _wfopen_s(&fh, path, L"r");
    }

    fseek(fh, 0, SEEK_END);
    auto length = ftell(fh);
    fseek(fh, 0, SEEK_SET);

    std::unique_ptr<char[]> data(new char[length + 1]);
    fread(data.get(), 1, length, fh);
    fclose(fh);

#if 0
    // dev, dont clear stack top

    nio::nop(0x18627CC14, 5);
    nio::nop(0x18627CC35, 5);
#endif

    bool result = !CScriptSystem_ExecuteLuaFile(g_ScriptSystem, data.get(), length, nullptr, false);

#if 0
    if (!result)
    {
        printf("Last error %s\n", lua_tolstring(state, -1, 0));
    }
#endif

    return result;
}

void Lua::LoadAllFiles()
{
    std::string path = Utility::GetAbsolutePathA() + "scripts";

    for (auto& entry : fs::recursive_directory_iterator(path))
    {
        if (!is_regular_file(entry))
            continue;

        if (entry.path().extension() == ".lua")
        {
            RunFile(entry.path().c_str());
        }
    }
}

static nomad::base_function init([]()
{
    auto loc = nio::pattern("53 56 57 48 83 EC 30 48 8B 79 38").first();
    nio::set_call(&lua_pcall, loc + 0xB2);
    nio::set_call(&lua_loadbuffer, loc + 0x65);

    // *shrug*
    loc = nio::pattern("84 C0 74 0B B8 ? ? ? ? 87 05 ? ? ? ? 0F B6 8F").first(0xCC);
    g_ScriptSystem = *(CScriptSystem**)((loc + 3) + *(int32_t*)(loc + 3) + 4);
    nio::set_call(&CScriptSystem_ExecuteLuaContext, loc + 0x1D);

    loc -= (0xC4 + 0xCC);

    loc = nio::get_call(loc);

    // dont really need the state getter, but lets collect it anyways
    nio::set_call(&GetLuaState, loc + 0x3A);
    nio::set_call(&lua_pushclosure, loc + 0x73);
    nio::set_call(&LuaSetFieldWrap, loc + 0x87);

    loc = nio::pattern("0F B6 F8 48 85 D2 74 27").first(-9);
    nio::set_call(&CScriptSystem_ExecuteLuaFile, loc);
    loc = *(BYTE*)CScriptSystem_ExecuteLuaFile == 0xE9 // not sure if such jumps will be in all versions
              ? nio::get_call((char*)CScriptSystem_ExecuteLuaFile)
              : (char*)CScriptSystem_ExecuteLuaFile; // thanks, "obfuscation"
    nio::set_call(&lua_gettop, ((char*)loc + 46));
    nio::set_call(&lua_tolstring, ((char*)loc + 215));

    // and we want to ensure the script dir presence too
    auto folder = (Utility::GetAbsolutePathW() + L"scripts").c_str();
    if (GetFileAttributesW(folder) == INVALID_FILE_ATTRIBUTES) CreateDirectoryW(folder, nullptr);
});
