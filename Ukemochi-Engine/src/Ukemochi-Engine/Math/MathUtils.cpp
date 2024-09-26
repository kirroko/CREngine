/* Start Header
*****************************************************************/
/*!
\file       MathUtils.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 20, 2024
\brief      This file contains the definition of the math utility functions.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "MathUtils.h"

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Get the absolute value of x.
    \param[in] x
     The input value to absolute.
    \return
     The absolute value.
    *************************************************************************/
    float abs(float x)
    {
        return x >= 0 ? x : -x;
    }
}