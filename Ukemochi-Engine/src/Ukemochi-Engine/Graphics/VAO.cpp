/*!
 * @file    VAO.cpp
 * @brief   This file contains the implementation of the VAO (Vertex Array Object) class, responsible for managing 
			vertex attributes and linking them to vertex buffer objects (VBOs) in OpenGL.
 *          It provides functionality to bind, unbind, delete VAOs, and link VBO attributes like position and color.
 *          The VAO helps manage the layout of vertex data and optimize the rendering pipeline in OpenGL.
 * @author  t.shunzhitomy
 * @date    25/09/2024
 */
#include "PreCompile.h"
#include"VAO.h"

 /*!
  * @brief Constructor that generates a Vertex Array Object (VAO) and assigns it an ID.
  *        This ID is used to reference the VAO in subsequent OpenGL operations.
  */
VAO::VAO() : ID(0)
{
	glGenVertexArrays(1, &ID);
}

/*!
 * @brief Links a Vertex Buffer Object (VBO) attribute, such as position or color, to the VAO.
 *        This function binds the VBO, specifies the layout of the vertex data, and enables the vertex attribute array.
 * @param VBO The VBO containing the vertex data to link.
 * @param layout The layout location of the attribute in the shader (e.g., 0 for position).
 * @param numComponents The number of components in the attribute (e.g., 3 for a vec3 position).
 * @param type The data type of the attribute (e.g., GL_FLOAT).
 * @param stride The stride between consecutive vertex attributes (e.g., the size of the entire vertex).
 * @param offset The offset within the VBO to the attribute data (e.g., where the position starts in a vertex).
 */
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, static_cast<GLsizei>(stride), offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::LinkAttribInteger(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribIPointer(layout, numComponents, type, static_cast<GLsizei>(stride), offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}
/*!
 * @brief Binds the VAO, making it the current active Vertex Array Object in OpenGL.
 */
void VAO::Bind()
{
	glBindVertexArray(ID);
}

/*!
 * @brief Unbinds the VAO, making no Vertex Array Object currently bound in OpenGL.
 */
void VAO::Unbind()
{
	glBindVertexArray(0);
}

/*!
 * @brief Deletes the VAO, freeing its allocated resources in OpenGL.
 */
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}