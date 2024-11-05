/*!
 * @file    EBO.h
 * @brief   This file contains the declaration of the EBO (Element Buffer Object) class, 
			responsible for managing OpenGL indices buffers used in rendering objects with glDrawElements().
 *          It provides functionality to bind, unbind, and delete the EBO.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */

#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>

/*!
 * @class EBO
 * @brief A class that encapsulates an Element Buffer Object (EBO) in OpenGL.
 *        The EBO stores indices that OpenGL uses to optimize rendering by reusing vertex data.
 */
class EBO
{
public:
	// ID reference of Elements Buffer Object
	GLuint ID;
	// Constructor that generates a Elements Buffer Object and links it to indices
	EBO(const GLuint* indices, GLsizeiptr size);

	// Binds the EBO
	void Bind();
	// Unbinds the EBO
	void Unbind();
	// Deletes the EBO
	void Delete();
};

#endif