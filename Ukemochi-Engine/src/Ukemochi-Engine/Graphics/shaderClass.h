/*!
 * @file    Shader.h
 * @brief   This file contains the declaration of the Shader class, responsible for compiling and managing OpenGL shaders (vertex and fragment shaders).
 *          It provides functionality to activate, delete, and set uniform values for shaders.
 *          The Shader class also includes utility functions to work with various types of uniforms such as bool, int, float, vectors, and matrices.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<../vendor/glm/glm/glm.hpp>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

/*!
 * @brief Reads the contents of a file and returns it as a string.
 * @param filename The path to the file.
 * @return A string containing the contents of the file.
 */
std::string get_file_contents(const char* filename);

/*!
 * @class Shader
 * @brief A class that encapsulates OpenGL shader program functionality, including compiling, activating, and deleting shaders.
 *        Provides utility functions to set uniform values in the shaders.
 */
class Shader {

public:
    /*!
     * @brief The ID of the shader program.
     */
	GLuint ID;

    /*!
     * @brief Constructor that compiles and links the vertex and fragment shaders from the provided file paths.
     * @param vertexFile The path to the vertex shader source file.
     * @param fragmentFile The path to the fragment shader source file.
     */
	Shader(const char* vertexFile, const char* fragmentFile);

    /*!
     * @brief Activates the shader program, making it the current shader in use by OpenGL.
     */
	void Activate();

    /*!
     * @brief Deactivates the shader program, making it the current shader in use by OpenGL.
     */
    void Deactivate();

    /*!
     * @brief Deletes the shader program, freeing its allocated resources in OpenGL.
     */
	void Delete();

    /*!
     * @brief Sets a boolean uniform in the shader program.
     * @param name The name of the uniform variable in the shader.
     * @param value The boolean value to set.
     */
	void setBool(const std::string& name, GLboolean value) const;

    /*!
     * @brief Sets an integer uniform in the shader program.
     * @param name The name of the uniform variable in the shader.
     * @param value The integer value to set.
     */
	void setInt(const std::string& name, GLint value) const;

    void setIntArray(const std::string& name, const GLint* values, GLint count) const;

    /*!
     * @brief Sets a floating-point uniform in the shader program.
     * @param name The name of the uniform variable in the shader.
     * @param value The float value to set.
     */
	void setFloat(const std::string& name, GLfloat value) const;

    /*!
     * @brief Sets a 2D vector uniform in the shader program.
     * @param name The name of the uniform variable in the shader.
     * @param value A glm::vec2 containing the values to set.
     */
    void setVec2(const std::string& name, const glm::vec2& value) const;

    /*!
    * @brief Sets a 2D vector uniform in the shader program using individual components.
    * @param name The name of the uniform variable in the shader.
    * @param x The x-component of the vector.
    * @param y The y-component of the vector.
    */
    void setVec2(const std::string& name, float x, float y) const;
    // ------------------------------------------------------------------------
    /*!
     * @brief Sets a 3D vector uniform in the shader program.
     * @param name The name of the uniform variable in the shader.
     * @param value A glm::vec3 containing the values to set.
     */
    void setVec3(const std::string& name, const glm::vec3& value) const;

    /*!
     * @brief Sets a 3D vector uniform in the shader program using individual components.
     * @param name The name of the uniform variable in the shader.
     * @param x The x-component of the vector.
     * @param y The y-component of the vector.
     * @param z The z-component of the vector.
     */
    void setVec3(const std::string& name, float x, float y, float z) const;
    // ------------------------------------------------------------------------
    /*!
     * @brief Sets a 4D vector uniform in the shader program.
     * @param name The name of the uniform variable in the shader.
     * @param value A glm::vec4 containing the values to set.
     */
    void setVec4(const std::string& name, const glm::vec4& value) const;

    /*!
     * @brief Sets a 4D vector uniform in the shader program using individual components.
     * @param name The name of the uniform variable in the shader.
     * @param x The x-component of the vector.
     * @param y The y-component of the vector.
     * @param z The z-component of the vector.
     * @param w The w-component of the vector.
     */
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    // ------------------------------------------------------------------------
    /*!
     * @brief Sets a 2x2 matrix uniform in the shader program.
     * @param name The name of the uniform variable in the shader.
     * @param mat A glm::mat2 representing the matrix to set.
     */
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    // ------------------------------------------------------------------------
    /*!
     * @brief Sets a 3x3 matrix uniform in the shader program.
     * @param name The name of the uniform variable in the shader.
     * @param mat A glm::mat3 representing the matrix to set.
     */
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    // ------------------------------------------------------------------------
    /*!
     * @brief Sets a 4x4 matrix uniform in the shader program.
     * @param name The name of the uniform variable in the shader.
     * @param mat A glm::mat4 representing the matrix to set.
     */
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};

#endif // SHADER_CLASS_H

