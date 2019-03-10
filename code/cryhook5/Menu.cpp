
/*
 * Copyright (C) Force67
 * Licensing information can be found in LICENSE.md
 * This file is part of CryHook5
 * Author: Force67
 * Started: 2019-03-07
 */

#include <Windows.h>
#include <imgui.h>
#include <imgui_tabs.h>

#include <vector>

#include <ScriptSystem.h>

#include <Utility/PathUtils.h>
#include <Menu.h>

static bool ScriptItemSelector(void* data, int idx, const char** out_text)
{
	const auto vec = (std::vector<std::string>*)data;

	if (out_text) *out_text = vec->at(idx).c_str();

	return true;
}

Menu *g_Menu{nullptr};
bool g_MenuActive{ false };

Menu::Menu()
{
	std::memset(&input_buffer, 0, 512);
	FindScripts();

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui::SetNextWindowPos(ImVec2(10, 10));

	auto &io = ImGui::GetIO();
	io.IniFilename = nullptr;

	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0;
	style.WindowRounding = 3;
	style.GrabRounding = 1;
	style.GrabMinSize = 20;
	style.FrameRounding = 3;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.47f, 0.81f, 0.82f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(1.00f, 0.00f, 0.00f, 0.65f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.82f, 0.62f, 0.47f, 0.18f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.33f, 0.33f, 0.27f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.29f, 0.30f, 0.66f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.70f, 0.37f, 0.05f, 0.73f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 0.00f, 0.00f, 0.27f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
	colors[ImGuiCol_Button] = ImVec4(0.79f, 0.28f, 0.18f, 0.46f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.77f, 0.26f, 0.15f, 0.43f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.40f, 0.00f, 0.62f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.33f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
	colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.50f, 0.50f, 0.33f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.50f, 0.50f, 0.47f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.70f, 0.70f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.04f, 0.10f, 0.09f, 0.51f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
}

void Menu::FindScripts()
{
	auto path = Utility::MakeAbsolutePathW(L"dawnhook\\scripts");

	WIN32_FIND_DATAW fd;
	HANDLE handle = FindFirstFileW((path+L"\\*.lua").c_str(), &fd);

	if (handle != INVALID_HANDLE_VALUE)
	{
		while (FindNextFileW(handle, &fd))
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				found_scripts.push_back(path + L"\\" + std::wstring(fd.cFileName));
			}
		}

		FindClose(handle);
	}
}

void Menu::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("CryHook5 1.2 by Force67", &g_MenuActive, 0);

	ImGui::BeginTabBar("Lua#left_tabs_bar");
	ImGui::DrawTabsBackground();
	if (ImGui::AddTab("Lua Editor"))
	{
		ImGui::InputTextMultiline("Enter Lua", input_buffer, 512);
		ImGui::NewLine();
		if (ImGui::Button("Execute"))
		{
			auto scr = CScriptSystem::GetInstance();

			if (!scr || !scr->ExecuteString(input_buffer, false))
			{
				std::puts("[CryHook5] : Failed to execute lua!\n");
			}
		}
	}

	if (ImGui::AddTab("Script Manager"))
	{
		if (ImGui::Button("Reload all scripts"))
		{
			FindScripts();

			for (const auto &file : found_scripts)
			{
				Lua::RunFile(file.c_str());
			}
		}

		static int current_item = 0;
		if (ImGui::ListBox("Load single\nscript", &current_item, ScriptItemSelector, &found_scripts, (int)found_scripts.size()))
		{
			auto dir = (Utility::MakeAbsolutePathW(L"cryhook5\\scripts")
				+ found_scripts[current_item]).
				c_str();
			Lua::RunFile(dir);
		}
	}

	ImGui::EndTabBar();
	ImGui::End();

	//ImGui::ShowStyleEditor();
}