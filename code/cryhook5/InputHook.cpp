/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <Nomad/nomad_base_function.h>
#include <Hooking.h>
#include <MinHook.h>

#include <imgui.h>

#include <Menu.h>
#include <ScriptSystem.h>

static BOOL (*PeekMessageA_Orig)(
    LPMSG lpMsg,
    HWND hWnd,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax,
    UINT wRemoveMsg
);

LRESULT ImGui_ImplWin32_WndProcHandler__(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui::GetCurrentContext() == nullptr)
        return 0;

    ImGuiIO& io = ImGui::GetIO();
    switch (msg)
    {
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
        {
            int button = 0;
            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 0;
            if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 1;
            if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 2;
            //    if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
            //        ::SetCapture(hwnd);
            io.MouseDown[button] = true;
            return 0;
        }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        {
            int button = 0;
            if (msg == WM_LBUTTONUP) button = 0;
            if (msg == WM_RBUTTONUP) button = 1;
            if (msg == WM_MBUTTONUP) button = 2;
            io.MouseDown[button] = false;
            //  if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
            //      ::ReleaseCapture();
            return 0;
        }
    case WM_MOUSEWHEEL:
        io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
    case WM_MOUSEHWHEEL:
        io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wParam < 256)
            io.KeysDown[wParam] = true;
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (wParam < 256)
            io.KeysDown[wParam] = false;
        return 0;
    case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
            io.AddInputCharacter((unsigned short)wParam);
        return 0;
    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT)
            return 1;
        return 0;
    }
    return 0;
}

static BOOL PostMessageA_Stub(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == 0x904 || Msg == 0x905)
    {
        printf("Game is setting cursor visibility!\n");
    }

    printf("Postmessagea %x\n", Msg);

    return 0;
    //return PostMessageA_Orig(hWnd, Msg, wParam, lParam);
}

static BOOL PeekMessageA_Stub(
    LPMSG lpMsg,
    HWND hWnd,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax,
    UINT wRemoveMsg
)
{
    BOOL result = PeekMessageA_Orig(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

    if (lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_F8)
    {
       // printf("pressed!\n");

		g_MenuActive = !g_MenuActive;

        //   ImGuiIO& io = ImGui::GetIO();
        //   io.MouseDrawCursor = input::g_showmenu;
    }

    if (g_MenuActive)
    {
        LRESULT imgui_handeled =
            ImGui_ImplWin32_WndProcHandler__(nullptr, lpMsg->message, lpMsg->wParam, lpMsg->lParam);

#if 1
        if (!imgui_handeled)
        {
            // grab mouse position
            ScreenToClient(static_cast<HWND>(hWnd), &lpMsg->pt);
            ImGuiIO& io = ImGui::GetIO();
            io.MousePos.x = static_cast<float>(lpMsg->pt.x);
            io.MousePos.y = static_cast<float>(lpMsg->pt.y);
        }
#endif

        lpMsg->message = 0;

        DispatchMessageA(lpMsg);

        return FALSE;
    }

    return result;
}

BOOL(*TranslateMessage_Orig)(const MSG*);

BOOL TranslateMessage_Stub(
    MSG *lpMsg
)
{
    BOOL result = TranslateMessage_Orig(lpMsg);

    if (lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_F8)
    {
        printf("pressed!\n");

		g_MenuActive = !g_MenuActive;

        //   ImGuiIO& io = ImGui::GetIO();
        //   io.MouseDrawCursor = input::g_showmenu;
    }

    if (g_MenuActive)
    {
        LRESULT imgui_handeled =
            ImGui_ImplWin32_WndProcHandler__(nullptr, lpMsg->message, lpMsg->wParam, lpMsg->lParam);

#if 1
        if (!imgui_handeled)
        {
            // grab mouse position
       //     ScreenToClient(static_cast<HWND>(hWnd), &lpMsg->pt);
            ImGuiIO& io = ImGui::GetIO();
            io.MousePos.x = static_cast<float>(lpMsg->pt.x);
            io.MousePos.y = static_cast<float>(lpMsg->pt.y);
        }
#endif

        lpMsg->message = 0;

        return FALSE;
    }

    return result;
}

static nomad::base_function init([]()
{
    //MH_CreateHookApi(L"User32.dll", "PeekMessageA", (LPVOID)&PeekMessageA_Stub, (LPVOID*)&PeekMessageA_Orig);
    MH_CreateHookApi(L"User32.dll", "TranslateMessage", (LPVOID)&TranslateMessage_Stub, (LPVOID*)&TranslateMessage_Orig);

    MH_EnableHook(nullptr);
});
