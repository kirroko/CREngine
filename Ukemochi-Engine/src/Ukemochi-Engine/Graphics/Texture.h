/*!
 * @file    Texture.h
 * @brief   This file contains the declaration of the Texture class, responsible for loading, binding, and managing textures in OpenGL.
 *          It includes functionality to bind textures to shaders and manage their lifecycle (e.g., creation, binding, and deletion).
 *          The class interacts with shaders to apply textures to objects during rendering.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */

#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include"shaderClass.h"

 /*!
  * @class Texture
  * @brief A class that encapsulates OpenGL texture functionality, providing methods to create, bind, and manage textures.
  */
class Texture
{
public:
	/*!
	 * @brief The OpenGL ID of the texture object.
	 */
	GLuint ID;

	/*!
	 * @brief The type of texture (e.g., GL_TEXTURE_2D).
	 */
	GLenum type;

	/*!
	 * @brief Constructor that loads and creates an OpenGL texture from an image file.
	 * @param image The path to the image file to load as a texture.
	 * @param texType The type of texture (e.g., GL_TEXTURE_2D).
	 * @param slot The texture unit slot to bind the texture to (e.g., GL_TEXTURE0).
	 * @param format The format of the image data (e.g., GL_RGBA or GL_RGB).
	 * @param pixelType The data type of the image's pixel data (e.g., GL_UNSIGNED_BYTE).
	 */
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

	// Assigns a texture unit to a texture
	void texUnit(Shader* shader, const char* uniform, GLuint unit);

	/*!
	 * @brief Binds the texture, making it active for subsequent OpenGL operations.
	 */
	void Bind();

	/*!
	* @brief Unbinds the texture, deactivating it for subsequent OpenGL operations.
	*/
	void Unbind();

	/*!
	* @brief Deletes the texture from OpenGL, freeing its resources.
	*/
	void Delete();
};
#endif