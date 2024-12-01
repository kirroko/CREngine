/* Start Header ************************************************************************/
/*!
\file       ProjectHandler.cpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Oct 26, 2024
\brief      This file generates .sln and .csproj for custom scripting.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "ProjectHandler.h"
#include <objbase.h>        // CoCreateGuid
#include <iomanip>          // setw, setfil
#include <sstream>          // ostringstream
#include <algorithm>

namespace Ukemochi
{
    /**
     * @brief Generate the C# project
     * @param projectPath The path to the project location
     */
    void ProjectHandler::GenerateProject(
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
    <ItemGroup>
        <Reference Include="UkemochiEngine">
            <HintPath>..\Ukemochi-Game\Resources\Scripts\Ukemochi-Scripting.dll</HintPath>
        </Reference>
    </ItemGroup>
</Project>
)";

        // Write csproject
        std::ofstream csprojFile(projectPath + "\\Assembly-CSharp.csproj");
        if (csprojFile.is_open())
        {
            csprojFile << csprojString;
            csprojFile.close();
        }
    }
}
