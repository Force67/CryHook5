project "CryHook5"
    language "C++"
    kind "SharedLib"
    
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
		--"../vendor/lua",
		"../vendor/minhook",
		"../vendor/imgui",
		"."
	}

	links
	{
		--"lua",
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
