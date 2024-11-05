/*!
 * @file    VAO.h
 * @brief   This file contains the declaration of the VAO (Vertex Array Object) class, responsible for managing vertex attributes and linking them to vertex buffer objects (VBOs) in OpenGL.
 *          The VAO class helps bind and manage vertex attributes like position, color, and texture coordinates in the rendering pipeline.
 *          It provides functionality to bind, unbind, and delete VAOs, as well as link VBO attributes.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */

#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"VBO.h"

 /*!
  * @class VAO
  * @brief A class that encapsulates a Vertex Array Object (VAO) in OpenGL.
  *        A VAO stores the layout of vertex attributes (e.g., position, color, texture coordinates) and manages their association with Vertex Buffer Objects (VBOs).
  */
class VAO
{
public:
	/*!
	 * @brief The OpenGL ID reference for the Vertex Array Object.
	 */
	GLuint ID;

	/*!
	 * @brief Constructor that generates a Vertex Array Object (VAO) and assigns it an ID.
	 */
	VAO();

	/*!
	 * @brief Links a Vertex Buffer Object (VBO) attribute, such as position or color, to the VAO.
	 * @param VBO The VBO containing the vertex data to link.
	 * @param layout The layout location of the attribute in the shader (e.g., 0 for position).
	 * @param numComponents The number of components in the attribute (e.g., 3 for a vec3 position).
	 * @param type The data type of the attribute (e.g., GL_FLOAT).
	 * @param stride The stride between consecutive vertex attributes (e.g., the size of the entire vertex).
	 * @param offset The offset within the VBO to the attribute data (e.g., where the position starts in a vertex).
	 */
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

	void LinkAttribInteger(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

	/*!
	 * @brief Binds the VAO, making it the current active Vertex Array Object in OpenGL.
	 */
	void Bind();

	/*!
	 * @brief Unbinds the VAO, making no Vertex Array Object currently bound.
	 */
	void Unbind();

	/*!
	 * @brief Deletes the VAO, freeing its resources in OpenGL.
	 */
	void Delete();
};

#endif // VAO_CLASS_H