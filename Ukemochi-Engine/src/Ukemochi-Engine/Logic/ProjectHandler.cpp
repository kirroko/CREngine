/* Start Header
*****************************************************************/
/*!
\file	ProjectHandler.cpp
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	26/10/24
\brief	This file generates .sln and .csproj for custom scripting

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "ProjectHandler.h"
#include <objbase.h>        // CoCreateGuid
#include <iomanip>          // setw, setfil
#include <sstream>          // ostringstream
#include <algorithm>

namespace Ukemochi
{
    // ================== PRIVATE FUNCTIONS ==================
    std::string ProjectHandler::GenerateGUID()
    {
        GUID guid;
        if (!CoCreateGuid(&guid))
        {
            UME_ENGINE_ERROR("Failed to generate GUID");
            return std::string{}; // return empty string
        }


        // Format the GUID as a string with braces
        std::ostringstream oss;
        oss << "{" << std::hex << std::uppercase
            << std::setw(8) << std::setfill('0') << guid.Data1 << "-"
            << std::setw(4) << std::setfill('0') << guid.Data2 << "-"
            << std::setw(4) << std::setfill('0') << guid.Data3 << "-";

        for (int i = 0; i < 2; ++i)
        {
            oss << std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[i]);
        }

        oss << "-";

        for (int i = 2; i < 8; ++i)
        {
            oss << std::setw(2) << std::setfill('0') << static_cast<int>(guid.Data4[i]);
        }

        oss << "}";
        return oss.str();
    }

    void ProjectHandler::replaceGUIDPlaceholder(std::string& slnString, const std::string& guid)
    {
        std::string placeholder = "{GUID}";
        size_t pos = 0;

        while ((pos = slnString.find(placeholder, pos)) != std::string::npos)
        {
            slnString.replace(pos, placeholder.length(), guid);
            pos += guid.length(); // Move past the last replacement
        }
    }

    std::string ProjectHandler::GetRelativepath(const std::string& from, const std::string& to)
    {
        std::filesystem::path fromPath(from);
        std::filesystem::path toPath(to);
        return std::filesystem::relative(toPath, fromPath).string();
    }

    // ================== PUBLIC FUNCTIONS ==================
    void ProjectHandler::GenerateSolutionAndProject(
        const std::string& projectPath)
    {
        UME_ENGINE_TRACE("Generating Project");
        std::string csprojString = R"(<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Library</OutputType>
    <TargetFramework>net472</TargetFramework>
    <Platforms>x64</Platforms>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <OutputPath>Temp\bin\Debug\</OutputPath>
    <PlatformTarget>x64</PlatformTarget>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <OutputPath>Temp\bin\Release\</OutputPath>
    <PlatformTarget>x64</PlatformTarget>
  </PropertyGroup>
  <ItemGroup>)";

        // Add each script file
        // for (const auto& script : scriptFiles)
        // {
        //     // std::string copyString(script);
        //     // size_t pos = copyString.find_first_of('.');
        //     // copyString = copyString.substr(0, pos);
        //     csprojString += "\n <Compile Include=\"" + script + "\" />\n";
        // }

        csprojString += R"(</ItemGroup>
    <ItemGroup>
        <Reference Include="UkemochiEngine">
            <HintPath>..\Ukemochi-Game\Resources\Scripts\Ukemochi-Scripting.dll</HintPath>
        </Reference>
    </ItemGroup>
</Project>
)";

        // Write csproject
        std::ofstream csprojFile(projectPath + "\\Assembly-CSharp.csproj");
        if(csprojFile.is_open())
        {
            csprojFile << csprojString;
            csprojFile.close();
        }
    }
}
