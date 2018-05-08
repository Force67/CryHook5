project "CryHook5"
    language "C++"
    kind "SharedLib"
	targetextension ".asi"
    
    vpaths
    {
        ["Headers/*"] = "**.hpp",
		["Headers/*"] = "**.h",
        ["Sources/*"] = "**.cpp",
        ["Sources/*"] = "**.c",
        ["*"] = "premake5.lua"
    }

	includedirs
	{
		"../shared",
		"./include",
		"../vendor/minhook",
		"../vendor/imgui",
	}

	links
	{
		"minhook",
		"imgui",
		"Shared"
	}

    files
    {
        "premake5.lua",
        "**.cpp",
        "**.hpp",        
        "**.h",      
        "**.c"
    }
