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
        ImGui::Begin("CryHook5 by Force67", &g_open, 0);

        ImGui::BeginTabBar("Lua#left_tabs_bar");
        ImGui::DrawTabsBackground();
        if (ImGui::AddTab("Lua Editor"))
        {
            ImGui::InputTextMultiline("Enter Lua", lua_buf, 512);
            ImGui::NewLine();
            if (ImGui::Button("Execute"))
            {
                auto scr = CScriptSystem::GetInstance();

                if (!scr->ExecuteString(lua_buf, false))
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

    // objective indicator

    // gamestate flag 0x21D00005E73 = paused

    //184D96590 + 16 = adduievent
    //return (*(int(__fastcall **)(signed __int64, __int64 *))(v23E89161380 + 16i64))(2467611153280i64, (__int64 *)&v40);
});
