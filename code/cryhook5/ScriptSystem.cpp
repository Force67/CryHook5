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

static CScriptSystem** g_ScriptSystem;

// lua script system stuff
static lua_State*(*GetCurrentLuaState)();
static int32_t (*LuaSetFieldWrap)(lua_State*, int64_t, const char*);

static bool (*CScriptSystem_ExecuteString)(CScriptSystem*, const char*, bool);
static bool (*CScriptSystem_ExecuteFile)(CScriptSystem*, const char*, size_t, const char*, bool);
static void (*CScriptSystem_RaiseError)(lua_State*, const char*, ...);

// lua
static void (*lua_pushclosure)(lua_State*, lua_CFunction, int32_t);

CScriptSystem* CScriptSystem::GetInstance()
{
    return *g_ScriptSystem;
}

bool CScriptSystem::ExecuteString(const char* code, bool raw)
{
    return CScriptSystem_ExecuteString(this, code, raw);
}

bool CScriptSystem::ExecuteFile(const char* code, size_t length, const char* idk, bool raw)
{
    return CScriptSystem_ExecuteFile(this, code, length, idk, raw);
}

void CScriptSystem::RaiseError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    CScriptSystem_RaiseError(GetCurrentLuaState(), fmt, args);

    va_end(args);
}


void Lua::RegisterFunction(const char* name, lua_CFunction fn)
{
    auto state = ::GetCurrentLuaState();

    lua_pushclosure(state, fn, 0);
    LuaSetFieldWrap(state, /*0xFFFFFFFE*/ 0xFFFFFFFE, name);
}

bool Lua::RunFile(const wchar_t* path)
{
    FILE* fh = nullptr;
    _wfopen_s(&fh, path, L"rb");

    if (!fh) return false;

    fseek(fh, 0, SEEK_END);
    auto length = ftell(fh);
    fseek(fh, 0, SEEK_SET);

    auto data = std::make_unique<char[]>(length + 1);
    data[length] = 0;
    fread(data.get(), 1, length, fh);
    fclose(fh);

#if 0
    // dev, dont clear stack top

    nio::nop(0x18627CC14, 5);
    nio::nop(0x18627CC35, 5);
#endif

    bool result = !CScriptSystem_ExecuteFile(*g_ScriptSystem, data.get(), length, nullptr, false);
    //bool result = !CScriptSystem_ExecuteString(*g_ScriptSystem, data.get(), false);

#if 0
    if (!result)
    {
        printf("Last error %s\n", lua_tolstring(state, -1, 0));
    }
#endif

    return result;
}

lua_State* Lua::GetCurrentLuaState()
{
    return ::GetCurrentLuaState();
}

static void BuildErrorString(char* ptr)
{
    // access the internal buffer of the
    // ndstring
    auto strptr = *(char **)(ptr + 8) + 12;

    printf(strptr);
}

static nomad::base_function init([]()
{
    char *loc = nullptr;

    auto matches = nio::pattern("89 70 30 48 8B 0D");

    if (matches.size() > 0)
    {
        loc = matches.first(3);

        g_ScriptSystem = (CScriptSystem**)((loc + 3) + *(int32_t*)(loc + 3) + 4);
        nio::set_call(&CScriptSystem_ExecuteString, loc + 0x1D);

        loc = nio::get_call(nio::pattern("57 48 83 EC 20 48 8B F9 E8 ? ? ? ? 33 D2 8D 4A 28").first(8));
    }
    else
    {
        loc = nio::pattern("84 C0 74 0B B8 ? ? ? ? 87 05 ? ? ? ? 0F B6 8F").first(0xCC);

        g_ScriptSystem = (CScriptSystem**)((loc + 3) + *(int32_t*)(loc + 3) + 4);
        nio::set_call(&CScriptSystem_ExecuteString, loc + 0x1D);

        loc -= 0xC4 + 0xCC;

        loc = nio::get_call(loc);
    }

    nio::set_call(&GetCurrentLuaState, loc + 0x3A);
    nio::set_call(&lua_pushclosure, loc + 0x73);
    nio::set_call(&LuaSetFieldWrap, loc + 0x87);

    loc = nio::pattern("0F B6 F8 48 85 D2 74 27").first(-9);

    nio::set_call(&CScriptSystem_ExecuteFile, loc);

    // hook a string building call inside CScriptSystem::RaiseError
    // however note, that not everything raises an error
    loc = nio::pattern("83 F8 02 74 37 48 8D 4C 24").first(44);

    // very aggressive inlining of the ndstring stuff btw
    nio::set_call(&CScriptSystem_RaiseError, loc);
    loc = (char*)CScriptSystem_RaiseError + 0x2E2;

    nio::put_call(loc, BuildErrorString);
});