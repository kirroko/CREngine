#include "PreCompile.h"
#include "shaderClass.h"

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
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

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

void Shader::Activate()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}

void Shader::setBool(const std::string& name, GLboolean value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, GLint value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}