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
    void ProjectHandler::GenerateSolutionAndProject(const std::string& solutionPath,
                                                    const std::string& projectPath,
                                                    const std::vector<std::string>& scriptFiles)
    {
        // Template can be found in template folder
        std::string slnString = R"(Microsoft Visual Studio Solution File, Format Version 12.00
Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "Assembly-CSharp", "Assembly-CSharp.csproj", "{GUID}"
EndProject
Global
    GlobalSection(SolutionConfigurationPlatforms) = preSolution
        Debug|Any CPU = Debug|Any CPU
        Release|Any CPU = Release|Any CPU
    EndGlobalSection
    GlobalSection(ProjectConfigurationPlatforms) = postSolution
        {GUID}.Debug|Any CPU.ActiveCfg = Debug|Any CPU
        {GUID}.Debug|Any CPU.Build.0 = Debug|Any CPU
        {GUID}.Release|Any CPU.ActiveCfg = Release|Any CPU
        {GUID}.Release|Any CPU.Build.0 = Release|Any CPU
    EndGlobalSection
EndGlobal)";

        // Generate a new GUID
        const std::string guid = GenerateGUID();

        // Replace the {GUID} placeholder with the actual GUID
        replaceGUIDPlaceholder(slnString, guid);

        // Write solution
        std::ofstream slnFile(solutionPath);
        if (slnFile.is_open())
        {
            slnFile << slnString;
            slnFile.close();
        }

        // Template can be found in template folder
        std::string csprojString = R"(<Project Sdk="Microsoft.NET.Sdk">
                                        <PropertyGroup>
                                            <OutputType>Library</OutputType>
                                            <TargetFramework>4.7.2</TargetFramework>
                                        </PropertyGroup>
                                        <ItemGroup>
                                    )";

        // Add each script file
        for (const auto& script : scriptFiles)
        {
            std::string relativePath = GetRelativepath(projectPath, script);
            csprojString += "       <Compile Include=\"" + relativePath + "\" />\n";
        }

        csprojString += R"(</ItemGroup>
    <ItemGroup>
        <Reference Include="UkemochiEngine">
            <HintPath>..\Ukemochi-Game\Resources\Scripts\Ukemochi-Scripting.dll</HintPath>
        </Reference>
    </ItemGroup>
</Project>
)";

        // Write csproject
        std::ofstream csprojFile(projectPath + "/Assembly-CSharp.csproj");
        if(csprojFile.is_open())
        {
            csprojFile << csprojString;
            csprojFile.close();
        }
    }
}
