workspace "Ukemochi"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
fmod_dll = "../Ukemochi-Engine/vendor/Fmod/lib/fmod.dll"

IncludeDir = {}
IncludeDir ["GLFW"] = "Ukemochi-Engine/vendor/GLFW/include"
IncludeDir ["Glad"] = "Ukemochi-Engine/vendor/Glad/include"
IncludeDir ["Fmod"] = "Ukemochi-Engine/vendor/Fmod/inc"
IncludeDir ["ImGui"] = "Ukemochi-Engine/vendor/imgui"


LibraryDir = {}
LibraryDir["Fmod"] = "Ukemochi-Engine/vendor/Fmod/lib"


include "Ukemochi-Engine/vendor/Fmod"
include "Ukemochi-Engine/vendor/GLFW"
include "Ukemochi-Engine/vendor/Glad"
include "Ukemochi-Engine/vendor/imgui"

project "Ukemochi-Engine"
	location "Ukemochi-Engine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


	pchheader "PreCompile.h"
	pchsource "Ukemochi-Engine/src/PreCompile.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.Fmod}",
		"%{IncludeDir.ImGui}"

	}
	
	libdirs
    {
        "%{LibraryDir.Fmod}" -- Add FMOD library directory
    }

	links
	{
		"GLFW",
		"Glad",
		"fmod_vc",
		"ImGui",
		"opengl32.lib",
	}
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		warnings "Extra"  -- Set warning level to 4 (equivalent to /W4)

		defines
		{
			"UME_PLATFORM_WINDOWS",
			"UME_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		buildoptions { "/wd6285", "/wd26451","/wd26498","/wd26800", "/wd4996", "/wd6031", "/wd4603" }  -- Disable specific warnings globally

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir.. "/Ukemochi-Game"),
			("{COPY} " .. fmod_dll .. " ../bin/" .. outputdir.. "/Ukemochi-Game"),
			("{COPYDIR} ../Assets ../bin/" .. outputdir .. "/Assets")
		}

	filter "configurations:Debug"
		defines "UME_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "UME_Release"
		optimize "On"
		
	filter "configurations:Dist"
		defines "UME_Dist"
		optimize "On"

project "Ukemochi-Game"
	location "Ukemochi-Game"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Ukemochi-Engine/vendor/spdlog/include",
		"Ukemochi-Engine/src"
	}

	links
	{
		"Ukemochi-Engine"
	}

	prebuildcommands 
    { 
        -- Copy the Ukemochi-Engine DLL before the build 
		"{COPY} ../bin/" .. outputdir .. "/Ukemochi-Engine/ukemochi-engine.dll ../bin/" .. outputdir .. "/Ukemochi-Game", 
		"{COPY} " .. fmod_dll .. " ../bin/" .. outputdir .. "/Ukemochi-Game" 
    }
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		warnings "Extra"  -- Set warning level to 4 (equivalent to /W4)

		buildoptions { "/wd6285", "/wd26451","/wd26498" }  -- Disable specific warnings globally
		
		defines
		{
			"UME_PLATFORM_WINDOWS"
		}


	filter "configurations:Debug"
		defines "UME_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "UME_Release"
		optimize "On"
		
	filter "configurations:Dist"
		defines "UME_Dist"
		optimize "On"