-- SAP build configuration script
-- Author(s): Force67
--
-- Copyright (c) 2015-2017 Dev-Force
--
-- Distributed under the MIT license (See accompanying file LICENSE or copy at
-- https://opensource.org/licenses/MIT)

-- This is the root build file for Premake. Premake will start processing by loading this
-- file, and recursively load all dependencies until all dependencies are either
-- resolved or known not to exist (which will cause the build to fail). So if
-- you add a new build file, there must be some path of dependencies from this
-- file to your new one or Premake won't know about it.

premake.path = premake.path .. ";build"

workspace "FC5_CryHook"
    configurations { "Debug", "Release" }

	targetdir '../bin/%{cfg.buildcfg}/'
	location "../build"
	platforms { "x64" }
    targetprefix ""
    buildoptions "/std:c++latest"
    --cppdialect "C++17"
    symbols "On"
    characterset "Unicode"
    -- Enable position-independent-code generation
    pic "On"
    systemversion "10.0.15063.0"
    startproject "luahook"
	
    -- Preprocessor definitions
    defines
    {
        "NOMINMAX",
        "WIN32_LEAN_AND_MEAN"
    }

    includedirs
    {
        "."
    }
	
	libdirs
	{
		"./shared/Lib",
	}

	
    filter "platforms:x64"
         architecture "x86_64"

    filter "configurations:Debug"
        defines { "GC_DBG" }

    filter "configurations:Release"
        --flags { "StaticRuntime" }
        optimize "Speed"

    filter {"system:windows", "configurations:Release", "kind:not StaticLib"}
        linkoptions "/PDB:\"symbols\\$(ProjectName).pdb\""

    filter { "system:windows", "kind:not StaticLib" }
        linkoptions "/manifestdependency:\"type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\""

    -- Disable deprecation warnings and errors
    filter "action:vs*"
        defines
        {
            "_CRT_SECURE_NO_WARNINGS",
            "_CRT_SECURE_NO_DEPRECATE",
            "_CRT_NONSTDC_NO_WARNINGS",
            "_CRT_NONSTDC_NO_DEPRECATE",
            "_SCL_SECURE_NO_WARNINGS",
            "_SCL_SECURE_NO_DEPRECATE"
            
            --"_WINSOCK_DEPRECATED_NO_WARNINGS"
        }

    group "Base"
	include "./cryhook5"
	
	group "Shared"
	include "./shared"

    group "Vendor"
	include "vendor/minhook"
	include "vendor/imgui"

	
-- Cleanup
if _ACTION == "clean" then
    os.rmdir("../bin");
    os.rmdir("../build");
end
