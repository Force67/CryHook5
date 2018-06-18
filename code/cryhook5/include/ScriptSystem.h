#pragma once

#if 0
extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#endif

using lua_State = __int64*;
using lua_CFunction = void(*)(lua_State);

namespace Lua
{
    lua_State *GetCurrentLuaState();

    void RegisterFunction(const char *name, lua_CFunction fn);
    bool RunFile(const char *path);

    bool TriggerEvent(const char *name, char *data, size_t data_size);
}

class CScriptSystem
{
    char pad[0x38];
    __int64* lua_state;
    
public:

    bool ExecuteString(const char*, bool);
    bool ExecuteFile(const char*, size_t, const char*, bool);
    static void RaiseError(const char *, ...);

    static CScriptSystem *GetInstance();
};