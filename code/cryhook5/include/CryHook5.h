#pragma once

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace gui
{
    void RenderMenus();
    void RenderGenerics();

    void ShowVisualError(const char*);
}

namespace input
{
    extern bool g_showmenu;
}