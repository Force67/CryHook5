
/*
 * Copyright (C) Force67
 * Licensing information can be found in LICENSE.md
 * This file is part of CryHook5
 * Author: Force67
 * Started: 2019-03-07
 */

#include <Nomad/nomad_base_function.h>
#include <Hooking.h>
#include <MinHook.h>

#include <imgui.h>
#include <Menu.h>

static BOOL (*SetCursorPos_O)(int, int);
static void (*UpdateRawInput)(void*, float);
static BOOL (*GetCursorPos_O)(LPPOINT);
static WNDPROC GameWndProc;

static void UpdateRawInput_Stub(void* self, float a2)
{
	// only update raw (dinput) if menu not visible
	if (!g_MenuActive)
		UpdateRawInput(self, a2);
}

static tagPOINT frozen;

static BOOL GetCursorPos_Stub(LPPOINT lpPoint)
{
	if (g_MenuActive)
	{
		memcpy(lpPoint, &frozen, sizeof(tagPOINT));
		return TRUE;
	}
	return GetCursorPos_O(lpPoint);
}

static LRESULT DawnHook_ProcessInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
				::SetCapture(hwnd);
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
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
				::ReleaseCapture();
			return 0;
		}
	case WM_MOUSEWHEEL:
		io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;
	case WM_MOUSEHWHEEL:
		io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
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

static LRESULT GameWndProc_Stub(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN && wParam == VK_F8)
	{
		g_MenuActive = !g_MenuActive;

		ImGuiIO& IO = ImGui::GetIO();

		// and freeze
		if (g_MenuActive)
		{
			// capture last mouse point we want
			GetCursorPos_O(&frozen);
			//GetCursorPos(&frozen);
			IO.MouseDrawCursor = true;
		}
		else
		{
			// restore from frozen value
			SetCursorPos_O(frozen.x, frozen.y);
			IO.MouseDrawCursor = false;
		}
	}

	if (g_MenuActive)
	{
		DawnHook_ProcessInput(hwnd, msg, wParam, lParam);

		// filter out movement msg
		if (msg == WM_MOUSEMOVE)
			msg = 0;
	}

	return GameWndProc(hwnd, msg, wParam, lParam);
}

BOOL SetCursorPos_Stub(int X, int Y)
{
	if (g_MenuActive)
	{
		return TRUE;
	}
	return SetCursorPos_O(X, Y);
}

static void (*update)(void*);

static void DoUpdate(char* k)
{
	if (!g_MenuActive)
		update(k);
	//else
	//*(tagPOINT*)(k + 132) = frozen;
}

static nomad::base_function init([]()
{
	// Step1: dont't track the cursor
	MH_CreateHookApi(L"User32.dll", "GetCursorPos", (LPVOID)&GetCursorPos_Stub, (LPVOID*)&GetCursorPos_O);
	MH_CreateHookApi(L"User32.dll", "SetCursorPos", (LPVOID)&SetCursorPos_Stub, (LPVOID*)&SetCursorPos_O);
	MH_EnableHook(nullptr);

	// Step2: install a wndproc listener
	char* loc = nio::pattern("FF 15 ? ? ? ? 48 8B D8 33 D2").first(28);
	GameWndProc = (WNDPROC)(loc + *(int32_t*)loc + 4);
	nio::writeVP<int32_t>(loc, (intptr_t)nio::AllocateFunctionStub(GameWndProc_Stub) - (intptr_t)loc - 4);

	// Step3: add a (shitty) check hook
	// in HID::InputManager::UpdateRaw?
	loc = nio::pattern("31 FF 48 C1 E8 20 C6 83").first(-95);
	MH_CreateHook(loc, &UpdateRawInput_Stub, (LPVOID*)&UpdateRawInput);
	MH_EnableHook(loc);

	// TODO: fix movement application once menu closed (ingame)
});
