/* Start Header ************************************************************************/
/*!
\file       ProjectHandler.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Oct 26, 2024
\brief      This file generates .sln and .csproj for custom scripting.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "PreCompile.h"


namespace Ukemochi
{
    class ProjectHandler
    {
    public:
        /**
         * @brief Helps dynamically generate a sln and csproj file
         * @param projectPath a projects path location
         */
        static void GenerateProject(const std::string& projectPath);
    };
}
