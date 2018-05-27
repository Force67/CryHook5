
/*
 *  This source file is part of the Far Cry 5 ScriptHook by Force67
 *  More information regarding licensing can be found in LICENSE.md
 */

#include <Nomad/nomad_base_function.h>
#include <Utility/PathUtils.h>
#include <Hooking.h>

#include <ScriptSystem.h>
#include <Game.h>

#include <CryHook5.h>

static CScriptSystem* g_ScriptSystem;

// lua script system stuff
static int64_t*(*GetCurrentLuaState)();
static int32_t (*LuaSetFieldWrap)(int64_t*, int64_t, const char*);

static bool (*CScriptSystem_ExecuteString)(CScriptSystem*, const char*, bool);
static bool (*CScriptSystem_ExecuteFile)(CScriptSystem*, const char*, size_t, const char*, bool);

// lua
static void (*lua_pushclosure)(int64_t*, Lua::lua_CFunction, int32_t);
static int32_t (*lua_gettop)(int64_t*);
static const char* (*lua_tolstring)(int64_t*, int64_t, int64_t);

CScriptSystem * CScriptSystem::GetInstance()
{
    return g_ScriptSystem;
}

bool CScriptSystem::ExecuteString(const char* code, bool raw)
{
    return CScriptSystem_ExecuteString(this, code, raw);
}

bool CScriptSystem::ExecuteFile(const char* code, size_t length, const char* idk, bool raw)
{
    return CScriptSystem_ExecuteFile(this, code, length, idk, raw);
}

void Lua::RegisterFunction(const char* name, lua_CFunction fn)
{
    auto state = GetCurrentLuaState();

    lua_pushclosure(state, fn, 0);
    LuaSetFieldWrap(state, 0xFFFFFFFE, name);
}

bool Lua::RunFile(const char* path)
{
    auto state = GetCurrentLuaState();

    FILE* fh = nullptr;
    fopen_s(&fh, path, "rb");

    if (!fh) return false;

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

    bool result = !CScriptSystem_ExecuteFile(g_ScriptSystem, data.get(), length, nullptr, false);

#if 0
    if (!result)
    {
        printf("Last error %s\n", lua_tolstring(state, -1, 0));
    }
#endif

    return result;
}

void BuildErrorString(char* ptr)
{
    // access the internal buffer of the
    // ndstring
    auto strptr = *(char **)(ptr + 8) + 12;

    printf(strptr);
}

static nomad::base_function init([]()
{
    // *shrug*
    auto loc = nio::pattern("84 C0 74 0B B8 ? ? ? ? 87 05 ? ? ? ? 0F B6 8F").first(0xCC);
    g_ScriptSystem = *(CScriptSystem**)((loc + 3) + *(int32_t*)(loc + 3) + 4);
    nio::set_call(&CScriptSystem_ExecuteString, loc + 0x1D);

    loc -= (0xC4 + 0xCC);

    loc = nio::get_call(loc);

    // dont really need the state getter, but lets collect it anyways
    nio::set_call(&GetCurrentLuaState, loc + 0x3A);
    nio::set_call(&lua_pushclosure, loc + 0x73);
    nio::set_call(&LuaSetFieldWrap, loc + 0x87);

    loc = nio::pattern("0F B6 F8 48 85 D2 74 27").first(-9);

    nio::set_call(&CScriptSystem_ExecuteFile, loc);
    loc = nio::get_call((char*)CScriptSystem_ExecuteFile);

    nio::set_call(&lua_gettop, ((char*)loc + 46));
    nio::set_call(&lua_tolstring, ((char*)loc + 215));

    // hook a string building call inside CScriptSystem::RaiseError
    // however note, that not everything raises an error
    loc = nio::pattern("A9 ? ? ? ? 0F 86 ? ? ? ? 48 8D 5D C0").first(-29);
    nio::put_call(loc, BuildErrorString);

    // and we want to ensure the script dir presence too
    auto folder = (Utility::GetAbsolutePathW() + L"scripts").c_str();
    if (GetFileAttributesW(folder) == INVALID_FILE_ATTRIBUTES) CreateDirectoryW(folder, nullptr);
});
