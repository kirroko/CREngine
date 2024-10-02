/* Start Header
*****************************************************************/
/*!
\file       Vector2D.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 08, 2024
\brief      This file contains the definition of the Vector2D structure.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "Vector2D.h" // for forward declaration
#include <cmath>      // for sqrt

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Addition assignment operator.
    \param[in] rhs
     The vector to add.
    \return
     A reference to the vector.
    *************************************************************************/
    Vector2D& Vector2D::operator+=(const Vector2D& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    /*!***********************************************************************
    \brief
     Subtraction assignment operator.
    \param[in] rhs
     The vector to subtract.
    \return
     A reference to the vector.
    *************************************************************************/
    Vector2D& Vector2D::operator-=(const Vector2D& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    /*!***********************************************************************
    \brief
     Multiplication assignment operator.
    \param[in] rhs
     The scalar to multiply.
    \return
     A reference to the vector.
    *************************************************************************/
    Vector2D& Vector2D::operator*=(float rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    /*!***********************************************************************
    \brief
     Division assignment operator.
    \param[in] rhs
     The scalar to divide.
    \return
     A reference to the vector.
    *************************************************************************/
    Vector2D& Vector2D::operator/=(float rhs)
    {
        if (rhs != 0.0f)
        {
            x /= rhs;
            y /= rhs;
        }
        return *this;
    }

    /*!***********************************************************************
    \brief
     Unary negation operator.
    \return
     The negated vector.
    *************************************************************************/
    Vector2D Vector2D::operator-() const
    {
        return Vector2D(-x, -y);
    }

    /*!***********************************************************************
    \brief
     Binary addition operator for two vectors.
    \param[in] lhs
     The first vector to add.
    \param[in] rhs
     The second vector to add.
    \return
     The vector of the sum of the two vectors.
    *************************************************************************/
    Vector2D operator+(const Vector2D& lhs, const Vector2D& rhs)
    {
        return Vector2D(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    /*!***********************************************************************
    \brief
     Binary subtraction operator for two vectors.
    \param[in] lhs
     The first vector to subtract.
    \param[in] rhs
     The second vector to subtract.
    \return
     The vector of the difference of the two vectors.
    *************************************************************************/
    Vector2D operator-(const Vector2D& lhs, const Vector2D& rhs)
    {
        return Vector2D(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    /*!***********************************************************************
    \brief
     Scalar multiplication operator for a vector and a scalar.
    \param[in] lhs
     The vector to multiply.
    \param[in] rhs
     The scalar to multiply.
    \return
     The vector of the product of the vector and scalar.
    *************************************************************************/
    Vector2D operator*(const Vector2D& lhs, float rhs)
    {
        return Vector2D(lhs.x * rhs, lhs.y * rhs);
    }

    /*!***********************************************************************
    \brief
     Scalar multiplication operator for a scalar and a vector.
    \param[in] lhs
     The scalar to multiply.
    \param[in] rhs
     The vector to multiply.
    \return
     The vector of the product of the scalar and vector.
    *************************************************************************/
    Vector2D operator*(float lhs, const Vector2D& rhs)
    {
        return Vector2D(lhs * rhs.x, lhs * rhs.y);
    }

    /*!***********************************************************************
    \brief
     Scalar division operator for a vector and a scalar.
    \param[in] lhs
     The vector to divide.
    \param[in] rhs
     The scalar to divide.
    \return
     The vector of the division of the vector and scalar.
    *************************************************************************/
    Vector2D operator/(const Vector2D& lhs, float rhs)
    {
        if (rhs == 0.0f)
            return lhs;
        return Vector2D(lhs.x / rhs, lhs.y / rhs);
    }

    /*!***********************************************************************
    \brief
     Normalize a vector.
    \param[out] pResult
     The normalized vector.
    \param[in] vec
     The input vector.
    *************************************************************************/
    void Vec2Normalize(Vector2D& pResult, const Vector2D& vec)
    {
        float length = Vec2Length(vec);
        if (length != 0.0f)
        {
            pResult.x = vec.x / length;
            pResult.y = vec.y / length;
        }
    }

    /*!***********************************************************************
    \brief
     Calculate the length of a vector.
    \param[in] vec
     The input vector.
    \return
     The length of the vector.
    *************************************************************************/
    float Vec2Length(const Vector2D& vec)
    {
        return sqrt(vec.x * vec.x + vec.y * vec.y);
    }

    /*!***********************************************************************
    \brief
     Calculate the squared length of a vector.
    \param[in] vec
     The input vector.
    \return
     The squared length of the vector.
    *************************************************************************/
    float Vec2SquareLength(const Vector2D& vec)
    {
        return vec.x * vec.x + vec.y * vec.y;
    }

    /*!***********************************************************************
    \brief
     Calculate the distance of two vectors.
    \param[in] lhs
     The first vector.
    \param[in] rhs
     The second vector.
    \return
     The distance of the two vectors.
    *************************************************************************/
    float Vec2Distance(const Vector2D& lhs, const Vector2D& rhs)
    {
        return Vec2Length(lhs - rhs);
    }

    /*!***********************************************************************
    \brief
     Calculate the squared distance of two vectors.
    \param[in] lhs
     The first vector.
    \param[in] rhs
     The second vector.
    \return
     The squared distance of the two vectors.
    *************************************************************************/
    float Vec2SquareDistance(const Vector2D& lhs, const Vector2D& rhs)
    {
        return Vec2SquareLength(lhs - rhs);
    }

    /*!***********************************************************************
    \brief
     Calculate the dot product of two vectors.
    \param[in] lhs
     The first vector.
    \param[in] rhs
     The second vector.
    \return
     The dot product of the two vectors.
    *************************************************************************/
    float Vec2DotProduct(const Vector2D& lhs, const Vector2D& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    /*!***********************************************************************
    \brief
     Calculate the magnitude of the cross product of two vectors.
    \param[in] lhs
     The first vector.
    \param[in] rhs
     The second vector.
    \return
     The magnitude of the cross product of the two vectors.
    *************************************************************************/
    float Vec2CrossProductMagnitude(const Vector2D& lhs, const Vector2D& rhs)
    {
        return lhs.x * rhs.y - lhs.y * rhs.x;
    }
}