/*!
 * @file    Shader.cpp
 * @brief   This file contains the implementation of the Shader class, responsible for loading, compiling, and managing OpenGL shader programs.
 *          It also provides utility functions to set uniform variables in the shader, including vectors and matrices.
 *          The shaders are used to control the rendering pipeline of OpenGL.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */
#include "PreCompile.h"
#include "shaderClass.h"

/*!
 * @brief Reads the contents of a file and returns it as a string.
 * @param filename The path to the file.
 * @return A string containing the contents of the file.
 * @throws If the file cannot be opened, throws the current errno as an exception.
 */
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno); // Throw an error if the file can't be opened
}

/*!
 * @brief Constructor that compiles the vertex and fragment shaders from provided source files.
 *        Links the compiled shaders into a shader program ready for use in OpenGL.
 * @param vertexFile The path to the vertex shader source file.
 * @param fragmentFile The path to the fragment shader source file.
 */
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str(); // Load vertex shader source
	const char* fragmentSource = fragmentCode.c_str(); // Load fragment shader source

	// Create a vertex shader object and specify its type as GL_VERTEX_SHADER
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Attach the vertex shader source code to the shader object
	// The 1 indicates the number of source code strings (in this case, just one)
	// &vertexShaderSource is the pointer to the source code string
	// NULL means the strings are null-terminated, so OpenGL can determine their length automatically
	glShaderSource(vertexShader, 1, &vertexSource, NULL);

	// Compile the vertex shader so that it can be used by the GPU
	glCompileShader(vertexShader);

	// Create a fragment shader object and specify its type as GL_FRAGMENT_SHADER
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Attach the fragment shader source code to the shader object
	// Same as with the vertex shader, the 1 indicates one source string and NULL means null-terminated
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

	// Compile the fragment shader so that it can be used by the GPU
	glCompileShader(fragmentShader);

	// Create a shader program object, which will link together the vertex and fragment shaders
	ID = glCreateProgram();

	// Attach the compiled vertex shader to the shader program
	glAttachShader(ID, vertexShader);

	// Attach the compiled fragment shader to the shader program
	glAttachShader(ID, fragmentShader);

	// Link the vertex and fragment shaders into a complete shader program
	// This step ensures that the shaders work together and are ready to be used for rendering
	glLinkProgram(ID);

	// Delete the vertex shader object now that it's linked into the shader program
	// This frees up memory since the shader is no longer needed after linking
	glDeleteShader(vertexShader);

	// Delete the fragment shader object for the same reason as above
	glDeleteShader(fragmentShader);
}

/*!
 * @brief Activates the shader program, making it the current shader in use by OpenGL.
 */
void Shader::Activate()
{
	glUseProgram(ID);
}

/*!
 * @brief Deletes the shader program, freeing its allocated resources in OpenGL.
 */
void Shader::Delete()
{
	glDeleteProgram(ID);
}

/*!
 * @brief Sets a boolean uniform in the shader program.
 * @param name The name of the uniform variable in the shader.
 * @param value The boolean value to set (0 or 1).
 */
void Shader::setBool(const std::string& name, GLboolean value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

/*!
 * @brief Sets an integer uniform in the shader program.
 * @param name The name of the uniform variable in the shader.
 * @param value The integer value to set.
 */
void Shader::setInt(const std::string& name, GLint value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

/*!
 * @brief Sets a floating-point uniform in the shader program.
 * @param name The name of the uniform variable in the shader.
 * @param value The float value to set.
 */
void Shader::setFloat(const std::string& name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

/*!
 * @brief Sets a 2D vector uniform in the shader program.
 * @param name The name of the uniform variable in the shader.
 * @param value A glm::vec2 containing the values to set.
 */
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

/*!
 * @brief Sets a 2D vector uniform in the shader program using individual components.
 * @param name The name of the uniform variable in the shader.
 * @param x The x-component of the vector.
 * @param y The y-component of the vector.
 */
void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
/*!
 * @brief Sets a 3D vector uniform in the shader program.
 * @param name The name of the uniform variable in the shader.
 * @param value A glm::vec3 containing the values to set.
 */
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

/*!
 * @brief Sets a 3D vector uniform in the shader program using individual components.
 * @param name The name of the uniform variable in the shader.
 * @param x The x-component of the vector.
 * @param y The y-component of the vector.
 * @param z The z-component of the vector.
 */
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
/*!
 * @brief Sets a 4D vector uniform in the shader program.
 * @param name The name of the uniform variable in the shader.
 * @param value A glm::vec4 containing the values to set.
 */
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

/*!
 * @brief Sets a 4D vector uniform in the shader program using individual components.
 * @param name The name of the uniform variable in the shader.
 * @param x The x-component of the vector.
 * @param y The y-component of the vector.
 * @param z The z-component of the vector.
 * @param w The w-component of the vector.
 */
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
/*!
 * @brief Sets a 2x2 matrix uniform in the shader program.
 * @param name The name of the uniform variable in the shader.
 * @param mat A glm::mat2 representing the matrix to set.
 */
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
/*!
 * @brief Sets a 3x3 matrix uniform in the shader program.
 * @param name The name of the uniform variable in the shader.
 * @param mat A glm::mat3 representing the matrix to set.
 */
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
/*!
 * @brief Sets a 4x4 matrix uniform in the shader program.
 * @param name The name of the uniform variable in the shader.
 * @param mat A glm::mat4 representing the matrix to set.
 */
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}