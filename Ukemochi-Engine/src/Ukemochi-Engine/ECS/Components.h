/* Start Header
*****************************************************************/
/*!
\file Components.h
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par junyukean.wong\@digipen.edu
\date 15/09/24
\brief Here is where we store all the different components that are needed to be added or removed (i.e Transform, Sprite, etc)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

namespace Ukemochi
{
    struct Transform
    {
        // Vec3?
        float position_x, position_y, position_z;
        float rotation_x, rotation_y, rotation_z;
        float scale_x, scale_y, scale_z;
    };
}