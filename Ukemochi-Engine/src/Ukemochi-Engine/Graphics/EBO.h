/* Start Header ************************************************************************/
/*!
\file       EBO.h
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Sept 25, 2024
\brief      This file contains the declaration of the EBO (Element Buffer Object) class, 
			responsible for managing OpenGL indices buffers used in rendering objects with glDrawElements().
 *          It provides functionality to bind, unbind, and delete the EBO.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

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