workspace "GAM200TESTENGINE"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir ["GLFW"] = "GAM200TESTENGINE/vendor/GLFW/include"

include "GAM200TESTENGINE/vendor/GLFW"

project "GAM200TESTENGINE"
	location "GAM200TESTENGINE"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "crpch.h"
	pchsource "GAM200TESTENGINE/src/crpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CR_PLATFORM_WINDOWS",
			"CR_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir.. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "CR_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "CR_Release"
		optimize "On"
		
	filter "configurations:Dist"
		defines "CR_Dist"
		optimize "On"

project "Sandbox"
	location "Sandbox"
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
		"GAM200TESTENGINE/vendor/spdlog/include",
		"GAM200TESTENGINE/src"
	}

	links
	{
		"GAM200TESTENGINE"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"CR_PLATFORM_WINDOWS"
		}


	filter "configurations:Debug"
		defines "CR_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "CR_Release"
		optimize "On"
		
	filter "configurations:Dist"
		defines "CR_Dist"
		optimize "On"