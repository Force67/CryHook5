#pragma once

#include <string>

namespace Lua
{
    using lua_CFunction = int32_t(*)(__int64 *);

    bool ExecuteLua(const char *code);
    void RegisterFunction(const char *name, lua_CFunction fn);
    bool RunFile(const wchar_t *path);

    void LoadAllFiles();
}