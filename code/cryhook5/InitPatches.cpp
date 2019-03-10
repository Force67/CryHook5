
/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <Nomad/nomad_base_function.h>
#include <Hooking.h>

static nomad::base_function init([]()
{
    auto matches = nio::pattern("44 8B 74 24 ? BF");

    if (!matches.empty())
    {
        char *loc = matches.first(-0xFB);

        // they've started using cstaticservicesp in 1.08, which breaks
        // our hook.

        // disable cstaticservicesp init
        nio::write<uint32_t>(loc, 0x90C300B0);  // mov al 0 ret nop
    }
});
