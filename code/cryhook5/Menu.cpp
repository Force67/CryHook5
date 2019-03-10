
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

	// monochrome theme, adapted from
	// https://gist.github.com/enemymouse/c8aa24e247a1d7b9fc33d45091cbb8f0
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 0.50f, 0.50f, 0.33f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.00f, 0.50f, 0.50f, 0.47f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.00f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.04f, 0.10f, 0.09f, 0.51f);
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
	ImGui::Begin("CryHook5 1.1 by Force67", &g_MenuActive, 0);

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
}