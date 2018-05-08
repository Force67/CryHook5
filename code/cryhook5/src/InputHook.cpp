
/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <Nomad/nomad_base_function.h>
#include <Hooking.h>

#include <imgui.h>

#include <CryHook5.h>

bool input::g_showmenu = false;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static WNDPROC GameWndProc_Orig;

// todo : fix cursor behaviour !
static LRESULT GameWndProc_Hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_KEYDOWN && wParam == VK_F8)
    {
        input::g_showmenu ^= true;
       // ShowCursor_CB(input::g_showmenu);
       // ImGui::GetIO().MouseDrawCursor = input::g_showmenu;
    }

    if (!input::g_showmenu) return GameWndProc_Orig(hwnd, msg, wParam, lParam);
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) return true;

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static nomad::base_function init([]()
{
    auto loc = nio::pattern("31 D2 48 8D 4C 24 ? 44 8D 42 50").first(16 + 3);

    // as it gets put into rax
    GameWndProc_Orig = (WNDPROC)(loc + *(int32_t*)loc + 4);
    *(int32_t*)loc = (intptr_t)(nio::AllocateFunctionStub(GameWndProc_Hook)) - (intptr_t)loc - 4;
});
