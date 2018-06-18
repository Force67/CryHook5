#pragma once

#include <Nomad/nomad_event.h>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace util
{
    extern nomad::nw_event<void> OnCHThreadProcess;
}

namespace gui
{
    void RenderMenus();
    void RenderGenerics();
}

namespace input
{
    extern bool g_showmenu;
}