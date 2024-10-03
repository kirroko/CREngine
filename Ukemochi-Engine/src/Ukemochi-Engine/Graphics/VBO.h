/*!
 * @file    VBO.h
 * @brief   This file contains the declaration of the VBO (Vertex Buffer Object) class, responsible for managing vertex data in OpenGL.
 *          The VBO class allows you to bind, unbind, and delete vertex data used in rendering. It stores the vertex attributes and sends them to the GPU.
 *          The class interacts with VAOs to manage vertex attribute data for rendering objects in OpenGL.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */

#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

 /*!
  * @class VBO
  * @brief A class that encapsulates an OpenGL Vertex Buffer Object (VBO), which stores vertex data and sends it to the GPU.
  *        A VBO is used in conjunction with a VAO to render objects in OpenGL.
  */
class VBO
{
public:
	/*!
	 * @brief The OpenGL ID reference for the Vertex Buffer Object.
	 */
	GLuint ID;
	/*!
	 * @brief Constructor that generates a Vertex Buffer Object (VBO) and links it to vertex data.
	 * @param vertices A pointer to the array of vertex data.
	 * @param size The size of the vertex data in bytes.
	 */
	VBO(GLfloat* vertices, GLsizeiptr size);

	/*!
	 * @brief Updates the vertex data in the VBO with new data.
	 * @param vertices A pointer to the new array of vertex data.
	 * @param size The size of the new vertex data in bytes.
	 */
	void UpdateData(GLfloat* vertices, GLsizeiptr size);

	/*!
	 * @brief Binds the VBO, making it the current active buffer in OpenGL.
	 */
	void Bind();

	/*!
	 * @brief Unbinds the VBO, making no Vertex Buffer Object currently bound.
	 */
	void Unbind();

	/*!
	 * @brief Deletes the VBO, freeing its resources in OpenGL.
	 */
	void Delete();
};

#endif