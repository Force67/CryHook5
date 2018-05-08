
/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <Windows.h>

#include <imgui.h>
#include <nomad/nomad_base_function.h>

#include <vector>

#include <LuaFunctions.h>
#include <CryHook5.h>

static bool g_open = true;

namespace gui
{
    static char lua_buf[512] = {0};
    static std::vector<const char*> g_errors;
    static ImVec2 g_windowsize;

    void RenderMenus()
    {
        // blur ? https://github.com/ocornut/imgui/issues/1117

        ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(10, 10));

        ImGui::Begin("CryHook5 by Force67", &g_open, 0);

        if (ImGui::CollapsingHeader("Lua Editor"))
        {
            ImGui::InputTextMultiline("Enter Lua", lua_buf, 512);
            ImGui::NewLine();
            ImGui::SameLine();
            if (ImGui::Button("Execute"))
            {
                if (!Lua::ExecuteLua(lua_buf))
                {
                    ShowVisualError("Failed to execute lua!\n");
                }
            }
        }

        if (ImGui::CollapsingHeader("Script Manager"))
        {
            if (ImGui::Button("Reload all scripts"))
            {
                Lua::LoadAllFiles();
            }
        }

        ImGui::End();
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
    RECT rect;
    GetWindowRect(FindWindowW(L"nomad", nullptr), &rect);
    gui::g_windowsize = ImVec2(rect.right - rect.left, rect.bottom - rect.top);

    // gamestate flag 0x21D00005E73 = paused
});
