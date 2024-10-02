/*!
 * @file    Texture.cpp
 * @brief   This file contains the implementation of the Texture class, responsible for loading, binding, and managing OpenGL textures.
 *          It includes functionality to bind textures to shaders and manage their lifecycle (e.g., creation, binding, and deletion).
 *          The class interacts with shaders to apply textures to objects during rendering.
 *          The STB image library is used to load image files into OpenGL textures.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */

#include "PreCompile.h"
#include"Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb/stb_image.h"

 /*!
  * @brief Constructor that loads and creates an OpenGL texture from an image file.
  *        The texture is assigned to a specific texture unit and configured with filtering and wrapping options.
  * @param image The path to the image file to load as a texture.
  * @param texType The type of texture (e.g., GL_TEXTURE_2D).
  * @param slot The texture unit slot to bind the texture to (e.g., GL_TEXTURE0).
  * @param format The format of the image data (e.g., GL_RGBA or GL_RGB).
  * @param pixelType The data type of the image's pixel data (e.g., GL_UNSIGNED_BYTE).
  */
Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	// Assigns the type of the texture ot the texture object
	type = texType;

	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	/*glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configures the way the texture repeats (if it does at all)
	/*glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	// Assigns the image to the OpenGL Texture object
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	// Generates MipMaps
	glGenerateMipmap(texType);

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texType, 0);
}

/*!
 * @brief Assigns a texture unit to a shader uniform.
 *        This function links the texture to the shader by specifying the texture unit.
 * @param shader The shader program to which the texture will be assigned.
 * @param uniform The name of the uniform variable in the shader.
 * @param unit The texture unit to assign (e.g., 0 for GL_TEXTURE0).
 */
void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Activate();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

/*!
 * @brief Binds the texture, making it active for subsequent OpenGL operations.
 */
void Texture::Bind()
{
	glBindTexture(type, ID);
}

/*!
 * @brief Unbinds the texture, deactivating it for subsequent OpenGL operations.
 */
void Texture::Unbind()
{
	glBindTexture(type, 0);
}

/*!
 * @brief Deletes the texture from OpenGL, freeing its resources.
 */
void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}