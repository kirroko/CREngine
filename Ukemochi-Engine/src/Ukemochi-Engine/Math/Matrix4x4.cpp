/* Start Header ************************************************************************/
/*!
\file       Matrix4x4.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Sept 09, 2024
\brief      This file contains the definition of the Matrix4x4 structure.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Matrix4x4.h" // for forward declaration
#include <cstring>     // for memcpy
#include <cmath>       // for sinf, cosf

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Constructor with a single array.
    \param[in] pArr
     The array of 16 floats.
    *************************************************************************/
    Matrix4x4::Matrix4x4(const float* pArr) : m{ 0.0f }
    {
        if (pArr)
            memcpy(m, pArr, sizeof(float) * 16);
        else
            Mtx44Identity(*this);
    }

    /*!***********************************************************************
    \brief
     Copy assignment operator.
    \param[in] rhs
     The matrix to copy.
    \return
     A reference to the matrix.
    *************************************************************************/
    Matrix4x4& Matrix4x4::operator=(const Matrix4x4& rhs)
    {
        if (this != &rhs)
            memcpy(m, rhs.m, sizeof(float) * 16);
        return *this;
    }

    /*!***********************************************************************
    \brief
     Multiplication assignment operator for two 4x4 matrices.
    \param[in] rhs
     The matrix to multiply.
    \return
     A reference to the matrix.
    *************************************************************************/
    Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& rhs)
    {
        Matrix4x4 result;
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                result.m2[row][col] = 0.0f;
                for (int i = 0; i < 4; ++i)
                    result.m2[row][col] += m2[row][i] * rhs.m2[i][col];
            }
        }
        *this = result;
        return *this;
    }

    /*!***********************************************************************
    \brief
     Binary multiplication operator for two 4x4 matrices.
    \param[in] lhs
     The first matrix to multiply.
    \param[in] rhs
     The second matrix to multiply.
    \return
     The matrix of the product of the two matrices.
    *************************************************************************/
    Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs)
    {
        Matrix4x4 result;
        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                result.m2[row][col] = 0.0f;
                for (int i = 0; i < 4; ++i)
                    result.m2[row][col] += lhs.m2[row][i] * rhs.m2[i][col];
            }
        }
        return result;
    }

    /*!***********************************************************************
    \brief
     Binary multiplication operator for a 4x4 matrix and a 3D vector.
    \param[in] lhs
     The matrix to multiply.
    \param[in] rhs
     The vector to multiply.
    \return
     The vector of the product of the matrix and vector.
    *************************************************************************/
    Vector3D operator*(const Matrix4x4& lhs, const Vector3D& rhs)
    {
        Vector3D result;
        result.x = lhs.m00 * rhs.x + lhs.m01 * rhs.y + lhs.m02 * rhs.z + lhs.m03;
        result.y = lhs.m10 * rhs.x + lhs.m11 * rhs.y + lhs.m12 * rhs.z + lhs.m13;
        result.z = lhs.m20 * rhs.x + lhs.m21 * rhs.y + lhs.m22 * rhs.z + lhs.m23;
        return result;
    }

    /*!***********************************************************************
    \brief
     Set the matrix to an identity matrix.
    \param[out] pResult
     The identity matrix.
    *************************************************************************/
    void Mtx44Identity(Matrix4x4& pResult)
    {
        pResult.m00 = 1.0f; pResult.m01 = 0.0f; pResult.m02 = 0.0f; pResult.m03 = 0.0f;
        pResult.m10 = 0.0f; pResult.m11 = 1.0f; pResult.m12 = 0.0f; pResult.m13 = 0.0f;
        pResult.m20 = 0.0f; pResult.m21 = 0.0f; pResult.m22 = 1.0f; pResult.m23 = 0.0f;
        pResult.m30 = 0.0f; pResult.m31 = 0.0f; pResult.m32 = 0.0f; pResult.m33 = 1.0f;
    }

    /*!***********************************************************************
    \brief
     Create a translation matrix.
    \param[out] pResult
     The translation matrix.
    \param[in] x
     The translation in the x-axis.
    \param[in] y
     The translation in the y-axis.
    \param[in] z
     The translation in the z-axis.
    *************************************************************************/
    void Mtx44Translate(Matrix4x4& pResult, float x, float y, float z)
    {
        Mtx44Identity(pResult);
        pResult.m03 = x;
        pResult.m13 = y;
        pResult.m23 = z;
    }

    /*!***********************************************************************
    \brief
     Create a rotation matrix around the x-axis with an angle in radians.
    \param[out] pResult
     The rotation matrix.
    \param[in] angle
     The angle of rotation in radians.
    *************************************************************************/
    void Mtx44RotXRad(Matrix4x4& pResult, float angle)
    {
        Mtx44Identity(pResult);
        pResult.m11 = cosf(angle);
        pResult.m12 = -sinf(angle);
        pResult.m21 = sinf(angle);
        pResult.m22 = cosf(angle);
    }

    /*!***********************************************************************
    \brief
     Create a rotation matrix around the y-axis with an angle in radians.
    \param[out] pResult
     The rotation matrix.
    \param[in] angle
     The angle of rotation in radians.
    *************************************************************************/
    void Mtx44RotYRad(Matrix4x4& pResult, float angle)
    {
        Mtx44Identity(pResult);
        pResult.m00 = cosf(angle);
        pResult.m02 = sinf(angle);
        pResult.m20 = -sinf(angle);
        pResult.m22 = cosf(angle);
    }

    /*!***********************************************************************
    \brief
     Create a rotation matrix around the z-axis with an angle in radians.
    \param[out] pResult
     The rotation matrix.
    \param[in] angle
     The angle of rotation in radians.
    *************************************************************************/
    void Mtx44RotZRad(Matrix4x4& pResult, float angle)
    {
        Mtx44Identity(pResult);
        pResult.m00 = cosf(angle);
        pResult.m01 = -sinf(angle);
        pResult.m10 = sinf(angle);
        pResult.m11 = cosf(angle);
    }

    /*!***********************************************************************
    \brief
     Create a scaling matrix.
    \param[out] pResult
     The scaling matrix.
    \param[in] x
     The scale factor in the x-axis.
    \param[in] y
     The scale factor in the y-axis.
    \param[in] z
     The scale factor in the z-axis.
    *************************************************************************/
    void Mtx44Scale(Matrix4x4& pResult, float x, float y, float z)
    {
        Mtx44Identity(pResult);
        pResult.m00 = x;
        pResult.m11 = y;
        pResult.m22 = z;
    }

    /*!***********************************************************************
    \brief
     Transpose the given matrix.
    \param[out] pResult
     The transposed matrix.
    \param[in] pMtx
     The input matrix to transpose.
    *************************************************************************/
    void Mtx44Transpose(Matrix4x4& pResult, const Matrix4x4& pMtx)
    {
        for (int row = 0; row < 4; ++row)
            for (int col = 0; col < 4; ++col)
                pResult.m2[row][col] = pMtx.m2[col][row];
    }
}