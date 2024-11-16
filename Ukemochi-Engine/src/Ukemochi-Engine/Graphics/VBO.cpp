/* Start Header ************************************************************************/
/*!
\file       VBO.cpp
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Sept 25, 2024
\brief      This file contains the implementation of the VBO (Vertex Buffer Object) class, responsible for managing vertex data in OpenGL.
            The VBO class allows you to create, bind, unbind, and delete vertex data used for rendering.
            It interacts with VAOs to manage vertex attribute data for rendering objects in OpenGL.
            This class sends vertex data to the GPU for use in rendering pipelines.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include"VBO.h"

 /*!
  * @brief Constructor that generates a Vertex Buffer Object (VBO) and links it to vertex data.
  *        The vertex data is stored in the GPU for subsequent rendering.
  * @param vertices A pointer to the array of vertex data.
  * @param size The size of the vertex data in bytes.
  */
VBO::VBO(const GLfloat* vertices, GLsizeiptr size)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
}

void VBO::UpdateData(const void* vertices, GLsizeiptr size)
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
	Unbind();
}

/*!
 * @brief Binds the VBO, making it the current active buffer in OpenGL.
 */
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

/*!
 * @brief Unbinds the VBO, making no Vertex Buffer Object currently bound in OpenGL.
 */
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*!
 * @brief Deletes the VBO, freeing its allocated resources in OpenGL.
 */
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}