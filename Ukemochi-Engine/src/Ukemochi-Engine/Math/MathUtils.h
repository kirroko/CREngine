/* Start Header
*****************************************************************/
/*!
\file       MathUtils.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 20, 2024
\brief      This file contains the declaration of the math utility functions.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

namespace Ukemochi
{
    const float PI = 3.14159265358979323846f;
    const float PIOVER180 = 0.0174532925199432f;

    /*!***********************************************************************
    \brief
     Get the min of two values.
    \param[in] T
     The type of input values.
    \param[in] lhs
     The first value.
    \param[in] rhs
     The second value.
    \return
     The smaller value of the two values.
    *************************************************************************/
    template <typename T>
    T min(T lhs, T rhs)
    {
        return (lhs < rhs) ? lhs : rhs;
    }

    /*!***********************************************************************
    \brief
     Get the max of two values.
    \param[in] T
     The type of input values.
    \param[in] lhs
     The first value.
    \param[in] rhs
     The second value.
    \return
     The larger value of the two values.
    *************************************************************************/
    template <typename T>
    T max(T lhs, T rhs)
    {
        return (lhs > rhs) ? lhs : rhs;
    }

    /*!***********************************************************************
    \brief
     Get the absolute value of x.
    \param[in] x
     The input value to absolute.
    \return
     The absolute value.
    *************************************************************************/
    float abs(float x);
}