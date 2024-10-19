/* Start Header
*****************************************************************/
/*!
\file       MathUtils.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 19, 2024
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
    template <typename T>
    static constexpr T PI = static_cast<T>(3.14159265358979323846);

    template <typename T>
    static constexpr T PIOVER180 = static_cast<T>(0.0174532925199432);

    /*!***********************************************************************
    \brief
     Get the min of two values.
    \tparam[in] T
     The type of input values.
    \param[in] lhs
     The first value.
    \param[in] rhs
     The second value.
    \return
     The smaller value of the two values.
    *************************************************************************/
    template <typename T>
    T min(const T& lhs, const T& rhs);

    /*!***********************************************************************
    \brief
     Get the max of two values.
    \tparam[in] T
     The type of input values.
    \param[in] lhs
     The first value.
    \param[in] rhs
     The second value.
    \return
     The larger value of the two values.
    *************************************************************************/
    template <typename T>
    T max(const T& lhs, const T& rhs);

    /*!***********************************************************************
    \brief
     Get the absolute value of x.
    \tparam[in] T
     The type of input value.
    \param[in] x
     The input value to absolute.
    \return
     The absolute value.
    *************************************************************************/
    template<typename T>
    T abs(const T& x);

    /*!***********************************************************************
    \brief
     Clamp a value between a minimum and maximum range.
    \tparam[in] T
     The type of input values.
    \param[in] value
     The value to be clamped.
    \param[in] min
     The minimum value.
    \param[in] max
     The maximum value.
    \return
     The clamped value within the range.
    *************************************************************************/
    template<typename T>
    T clamp(const T& value, const T& min, const T& max);

    /*!***********************************************************************
    \brief
     Convert a value from radian to degree.
    \tparam[in] T
     The type of input value.
    \param[in] radian
     The value to be converted.
    \return
     The value in degree.
    *************************************************************************/
    template<typename T>
    T degree(const T& radian);

    /*!***********************************************************************
    \brief
     Convert a value from degree to radian.
    \tparam[in] T
     The type of input value.
    \param[in] degree
     The value to be converted.
    \return
     The value in radian.
    *************************************************************************/
    template<typename T>
    T radian(const T& degree);
}

#include "MathUtils.tpp"