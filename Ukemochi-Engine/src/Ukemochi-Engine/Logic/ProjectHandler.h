/* Start Header
*****************************************************************/
/*!
\file	ProjectHandler.h
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
#pragma once
#include "PreCompile.h"


namespace Ukemochi
{
    class ProjectHandler
    {
        /**
         * @brief Helps to generate a GUID
         * @return the GUID in string format. Will return an empty string if GUID failed to create
         */
        static std::string GenerateGUID();

        /**
         * @brief Helper function to replace {GUID} to the GUID parameter
         * @param slnString the template slnString
         * @param guid the GUID
         */
        static void replaceGUIDPlaceholder(std::string &slnString, const std::string &guid);

        /**
         * @brief Get the relative path from one file to another
         * @param from from file
         * @param to to file
         * @return string of the relative path
         */
        static std::string GetRelativepath(const std::string& from, const std::string& to);
    public:
        /**
         * @brief Helps dynamically generate a sln and csproj file
         * @param solutionPath a solutions path location
         * @param projectPath a projects path location
         * @param scriptFiles a container of scripts location
         */
        static void GenerateSolutionAndProject(const std::string& solutionPath, const std::string& projectPath, const std::vector<std::string>& scriptFiles);
    };
}
