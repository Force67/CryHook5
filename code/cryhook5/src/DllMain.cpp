
/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <Windows.h>
#include <cstdio>

#include <Nomad/nomad_event.h>
#include <Nomad/nomad_base_function.h>

#include <Hooking.h>

#include <MinHook.h>

namespace util
{
    nomad::nw_event<void> OnCHThreadProcess;
}

void OpenConsole()
{
    // Console
    AllocConsole();
    AttachConsole(GetCurrentProcessId());

    // Console Window
    const HWND hConsole = GetConsoleWindow();
    RECT rect;
    GetWindowRect(hConsole, &rect);

    if (GetSystemMetrics(SM_CMONITORS) > 1)
    {
        // Desktop
        HWND hDesktop = GetDesktopWindow();
        RECT rDesktopRect;
        GetWindowRect(hDesktop, &rDesktopRect);
        SetWindowPos(hConsole, nullptr, rDesktopRect.right + 100, 50, rect.right - rect.left, rect.bottom - rect.top,
            0);
    }

    freopen("CON", "w", stdout);
    freopen("CONIN$", "r", stdin);
}

DWORD WINAPI CH_Thread(LPVOID lpParam)
{
    auto base = GetModuleHandle(L"FC_m64.dll");

    OpenConsole();
    MH_Initialize();
    nio::set_base((uintptr_t)base);

    nomad::base_function::run_all();

    while (true)
    {
        Sleep(30);
        util::OnCHThreadProcess();
    }

    return 0;
}

BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        MessageBoxA(0, "break!", 0, 0);

        // need thread context to call d3d create
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CH_Thread, hinstDLL, NULL, NULL);
        return true;
    }

    return true;
}