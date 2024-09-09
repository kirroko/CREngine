#include "VBO.h"

VBO::VBO(GLfloat* vertices, GLsizeiptr size)
{
    // Creates vertex buffer object as place to store the vertex informtion
    // Buffer is a block of memory that stores data
	glGenBuffers(1, &ID);
    // Binding a buffer, like VBO, to GL_ARRAY_BUFFER, you're selecting that specific buffer as the place where OpenGL will look for or store vertex data. 
    // This is important because OpenGL needs to know where in the GPU memory it should operate.
    // GL_ARRAY_BUFFER specifies that the buffer will hold vertex attribute data.
    // And its telling the program "Hey, I want to use this space to store things."
    glBindBuffer(GL_ARRAY_BUFFER, ID);

    // glBufferData copies the data from vertices into the space previously allocated
    // GL_ARRAY_BUFFER specifies that the buffer will hold vertex attribute data.
    // sizeof(vertices) calculates how much memory is required to store the entire array of vertex data.
    // vertices is a pointer to the actual data you want to send to the GPU.In this case, vertices is an array of vertex data this data is copied from the CPU to the GPU.
    // GL_STATIC_DRAW tells OpenGL that the data will be set once and used many times without being changed. 
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
    glDeleteBuffers(1, &ID);
}