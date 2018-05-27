
/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <nomad/nomad_base_function.h>

#include <Hooking.h>
#include <MinHook.h>
#include <stdio.h>

#include <ScriptSystem.h>

#if 0

static FILE *g_file;
static char *latestsub;

static void(*lua_pushclosure_orig)(__int64*, Lua::lua_CFunction, int32_t);
static int(*func_pushstringthing)(__int64 *, __int64, const char *);
static void(*lua_regnamespace)(const char*);

static void lua_pushclosure_orig_hook(__int64 * a1, Lua::lua_CFunction a2, int32_t a3)
{
    latestsub = (char*)a2;

    lua_pushclosure_orig(a1, a2, a3);
}

static int func_pushstringthing_hook(__int64 * a1, __int64 a2, const char *a3)
{
#if 0
    fprintf(g_file, "MakeName	(0X%p, \"SCR_%s\");\n", latestsub, a3);
#endif

    fprintf(g_file, "{ \"name\":\"%s\", \"addr\":\"0x%p\" },\n", a3, latestsub);

    fflush(g_file);

    return func_pushstringthing(a1, a2, a3);
}

void __fastcall sub_182E26300(const char *ptr)
{
    fprintf(g_file, "\n \"%s\": [", ptr);
    fflush(g_file);

    lua_regnamespace(ptr);
}

static nomad::base_function init([]()
{
    // very primitive way of "dumping" lua functions
    // TODO : libudis: find real handlers more easily
    g_file = fopen("fc5lua_json.json", "w");

#if 0
    fprintf(g_file,
        "#include <idc.idc>\n"
        "static main(void) {\n");
#endif

    auto loc = nio::get_call(nio::pattern("B2 01 48 89 D9 E8 ? ? ? ? 48 8B 4B 58").first(24));
    MH_CreateHook(loc, &lua_pushclosure_orig_hook, (void**)&lua_pushclosure_orig);
    MH_EnableHook(loc);

    loc = nio::get_call(nio::pattern("E9 ? ? ? ? 48 83 C4 20 5B C3 CC 8B 04 24").first());
    MH_CreateHook(loc, &func_pushstringthing_hook, (void**)&func_pushstringthing);
    MH_EnableHook(loc);

    loc = (char*)0x182E26300;
    MH_CreateHook(loc, &sub_182E26300, (void**)&lua_regnamespace);
    MH_EnableHook(loc);

});

#endif