/* Start Header ************************************************************************/
/*!
\file       EBO.cpp
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Sept 25, 2024
\brief      This file contains the implementation of the EBO (Element Buffer Object) class, 
			responsible for managing OpenGL indices buffers used in rendering objects with glDrawElements().
			It provides functionality to bind, unbind, and delete the EBO.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "EBO.h"

/*!
 * @brief Constructor that generates an Element Buffer Object (EBO) and links it to the provided indices.
 * @param indices Pointer to the array of indices that define the order in which vertices are rendered.
 * @param size The size of the indices array in bytes.
 */
EBO::EBO(const GLuint* indices, GLsizeiptr size)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

/*!
 * @brief Binds the EBO, making it the current active Element Buffer Object in OpenGL.
 */
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

/*!
 * @brief Unbinds the EBO, making no Element Buffer Object currently bound.
 */
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*!
 * @brief Deletes the EBO, freeing its allocated resources in OpenGL.
 */
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}