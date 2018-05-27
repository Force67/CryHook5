#pragma once

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace gui
{
    void RenderMenus();
    void RenderGenerics();
}

namespace input
{
    extern bool g_showmenu;
}