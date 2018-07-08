/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <imgui.h>
#include <imgui_tabs.h>
#include <nomad/nomad_base_function.h>

#include <vector>

#include <ScriptSystem.h>
#include <CryHook5.h>

#include <Utility/PathUtils.h>

static bool g_open = true;

namespace gui
{
    static char lua_buf[512] = {0};
    static std::vector<std::string> g_scripts;

    void FindScripts()
    {
        std::string path = Utility::GetAbsolutePathA() + "scripts";

        for (auto& entry : fs::recursive_directory_iterator(path))
        {
            if (!is_regular_file(entry))
                continue;

            if (entry.path().extension() == ".lua")
            {
                auto full_p = entry.path().generic_string();
                g_scripts.push_back(entry.path().generic_string().substr(path.length() + 1));
            }
        }
    }

    static bool ScriptItemSelector(void* data, int idx, const char** out_text)
    {
        const auto vec = (std::vector<std::string>*)data;

        if (out_text) *out_text = vec->at(idx).c_str();

        return true;
    }

    void RenderMenus()
    {
        // blur ? https://github.com/ocornut/imgui/issues/1117

        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("CryHook5 1.1 by Force67", &g_open, 0);

        ImGui::BeginTabBar("Lua#left_tabs_bar");
        ImGui::DrawTabsBackground();
        if (ImGui::AddTab("Lua Editor"))
        {
            ImGui::InputTextMultiline("Enter Lua", lua_buf, 512);
            ImGui::NewLine();
            if (ImGui::Button("Execute"))
            {
                auto scr = CScriptSystem::GetInstance();

                if (!scr || !scr->ExecuteString(lua_buf, false))
                {
                    printf("[CryHook5] : Failed to execute lua!\n");
                }
            }
        }

        if (ImGui::AddTab("Script Manager"))
        {
            if (ImGui::Button("Reload all scripts"))
            {
                FindScripts();

                for (const auto &file : gui::g_scripts)
                {
                    Lua::RunFile(file.c_str());
                }
            }

            static int current_item = 0;
            if (ImGui::ListBox("Load single\nscript", &current_item, ScriptItemSelector, &g_scripts, (int)g_scripts.size()))
            {
                auto dir = (Utility::GetAbsolutePathA()
                    + "scripts\\" 
                    + g_scripts[current_item]).
                    c_str();
                Lua::RunFile(dir);
            }
        }

#if 0
        if (ImGui::AddTab("Asset Manager"))
        {
 
        }
#endif

        ImGui::EndTabBar();

#if 0
        if (ImGui::CollapsingHeader("Settings"))
        {
            ImGui::BeginTabBar("Customization#left_tabs_bar");
            ImGui::DrawTabsBackground();

            if (ImGui::AddTab("Menu Colors"))
            {
            }

            if (ImGui::AddTab("ScriptManager Settings"))
            {
            }

            ImGui::EndTabBar();
        }
#endif

        ImGui::End();

      //  ImGui::ShowStyleEditor();
    }

    void RenderGenerics()
    {
    }
}

static nomad::base_function init([]()
{
    gui::FindScripts();

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

    // objective indicator

    // gamestate flag 0x21D00005E73 = paused

    //184D96590 + 16 = adduievent
    //return (*(int(__fastcall **)(signed __int64, __int64 *))(v23E89161380 + 16i64))(2467611153280i64, (__int64 *)&v40);
});
