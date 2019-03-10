#pragma once

/*
 * Copyright (C) Force67
 * Licensing information can be found in LICENSE.md
 * This file is part of CryHook5
 * Author: Force67
 * Started: 2019-03-07
 */

#include <vector>

class Menu
{
	std::vector<std::wstring> found_scripts;
	char input_buffer[512];

	void FindScripts();

public:

	Menu();

	void Draw();
};

extern Menu *g_Menu;
extern bool g_MenuActive;