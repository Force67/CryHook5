#pragma once

#include <stdint.h>

namespace Lua
{
    using lua_CFunction = int32_t(*)(__int64 *);

    void RegisterFunction(const char *name, lua_CFunction fn);
    bool RunFile(const char *path);
}

class CScriptSystem
{
    char pad[0x38];
    __int64* lua_state;
    
public:

    bool ExecuteString(const char*, bool);
    bool ExecuteFile(const char*, size_t, const char*, bool);

    static CScriptSystem *GetInstance();
};