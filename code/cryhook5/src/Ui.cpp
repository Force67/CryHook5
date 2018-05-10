/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <imgui.h>
#include <imgui_tabs.h>
#include <nomad/nomad_base_function.h>

#include <vector>

#include <LuaFunctions.h>
#include <CryHook5.h>

#include <Utility/PathUtils.h>

static bool g_open = true;

namespace gui
{
    static char lua_buf[512] = {0};
    static std::vector<const char*> g_errors;
    static ImVec2 g_windowsize;
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
        ImGui::Begin("CryHook5 by Force67", &g_open, 0);

        ImGui::BeginTabBar("Lua#left_tabs_bar");
        ImGui::DrawTabsBackground();
        if (ImGui::AddTab("Lua Editor"))
        {
            ImGui::InputTextMultiline("Enter Lua", lua_buf, 512);
            ImGui::NewLine();
            if (ImGui::Button("Execute"))
            {
                if (!Lua::ExecuteLua(lua_buf))
                {
                    ShowVisualError("Failed to execute lua!\n");
                }
            }
        }

        if (ImGui::AddTab("Script Manager"))
        {
            if (ImGui::Button("Reload all scripts"))
            {
                Lua::LoadAllFiles();
            }

            static int current_item = 0;
            if (ImGui::ListBox("Load single\nscript", &current_item, ScriptItemSelector, &g_scripts, (int)g_scripts.size()))
            {
                // yes i know its ugly but, imgui doesnt support wide strings
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

                auto dir = (Utility::GetAbsolutePathW()
                    + L"scripts\\" 
                    + converter.from_bytes(g_scripts[current_item])).
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
        /* ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiSetCond_Always);
         ImGui::Begin("Background", nullptr, g_windowsize, 0.f,
                      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove |
                      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
 
         ImGui::GetWindowDrawList()->AddText(
             ImVec2(0.120f * g_windowsize.x, 0.150f * g_windowsize.y),
             ImColor(0xFF, 0, 0xFF, 0xFF), "CAPTIEN !");
 
         for (size_t i = 0; i < g_errors.size(); i++)
         {
         //    ImGui::GetWindowDrawList()->AddText(
         //        { g_windowsize.x - 300.0f, g_windowsize.y - 10.0f /*+ (i * 5.0f)},
         //        ImColor(0xFF, 0xFF, 0, 0xFF), g_errors[i]);
         }
 
         ImGui::End();*/
    }

    void ShowVisualError(const char* error)
    {
        g_errors.push_back(error);
    }
}

static nomad::base_function init([]()
{
    gui::FindScripts();
    // gamestate flag 0x21D00005E73 = paused
});
