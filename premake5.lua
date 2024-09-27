workspace "Ukemochi"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir ["GLFW"] = "Ukemochi-Engine/vendor/GLFW/include"
IncludeDir ["Glad"] = "Ukemochi-Engine/vendor/Glad/include"
IncludeDir ["Fmod"] = "Ukemochi-Engine/vendor/Fmod/inc"
LibraryDir = {}
LibraryDir["Fmod"] = "Ukemochi-Engine/vendor/Fmod/lib"

include "Ukemochi-Engine/vendor/GLFW"
include "Ukemochi-Engine/vendor/Glad"
include "Ukemochi-Engine/vendor/Fmod"

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
		"%{IncludeDir.Fmod}"

	}
	
	libdirs
    {
        "%{LibraryDir.Fmod}" -- Add FMOD library directory
    }

	links
	{
		"GLFW",
		"Glad",
		"opengl32.lib",
		"fmod_vc"
	}
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"UME_PLATFORM_WINDOWS",
			"UME_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir.. "/Ukemochi-Game")
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

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

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