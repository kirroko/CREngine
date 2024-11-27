/*!***********************************************************************
\file       ColorBufferBatchRendering.cpp
\author     Tan Shun Zhi Tomy, t.shunzhitomy@digipen.edu
\date       26/11/2024
\brief
This file contains the implementation of the `ColorBufferBatchRenderer2D`
class. The class facilitates efficient batch rendering of debug boxes
with customizable positions, sizes, colors, and rotations using OpenGL.

The batch renderer optimizes rendering by grouping multiple debug boxes
into a single draw call, leveraging vertex and index buffers. It is used
primarily for debugging and visualization purposes in graphical applications.

\details
- The `init` function sets up the required vertex, index buffers, and attributes.
- The `drawDebugBox` function adds a box to the current batch.
- The `flush` function sends the batch to the GPU for rendering.
- The system minimizes draw calls by batching up to `maxBufferSprites`
  boxes per draw call.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*************************************************************************/

#include "PreCompile.h"
#include "ColorBufferBatchRendering.h"
#include "shaderClass.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"
#include "Texture.h"

/*!***********************************************************************
\brief
Constructs a new `ColorBufferBatchRenderer2D` object.
*************************************************************************/
ColorBufferBatchRenderer2D::ColorBufferBatchRenderer2D(){}

/*!***********************************************************************
\brief
Destructor for the `ColorBufferBatchRenderer2D` object. Cleans up
resources such as VAO, VBO, EBO, and clears the vertex and index buffers.
*************************************************************************/
ColorBufferBatchRenderer2D::~ColorBufferBatchRenderer2D()
{
	if (color_buffer_vao) color_buffer_vao->Delete();
	if (color_buffer_vbo) color_buffer_vbo->Delete();
	if (color_buffer_ebo) color_buffer_ebo->Delete();
	indices.clear();
	vertices.clear();
}

/*!***********************************************************************
\brief
Initializes the batch renderer with a shader and sets up the necessary
vertex and index buffers.

\param colorBufferShader
The shader to be used for rendering the color buffer.
*************************************************************************/
void ColorBufferBatchRenderer2D::init(std::shared_ptr<Shader> colorBufferShader)
{
	color_buffer_vao = std::make_unique<VAO>();
	color_buffer_vao->Bind();

	color_buffer_vbo = std::make_unique<VBO>(nullptr, sizeof(ColorBufferVertex) * maxBufferSprites * 4);
	color_buffer_vbo->Bind();

	color_buffer_vao->LinkAttrib(*color_buffer_vbo, 0, 3, GL_FLOAT, sizeof(ColorBufferVertex), (void*)offsetof(ColorBufferVertex, position));
	color_buffer_vao->LinkAttrib(*color_buffer_vbo, 1, 3, GL_FLOAT, sizeof(ColorBufferVertex), (void*)offsetof(ColorBufferVertex, color));

    // Populate indices
    int offset = 0;
    for (int i = 0; i < maxBufferSprites; ++i) 
    {
        indices.push_back(offset + 0); // First triangle
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);

        indices.push_back(offset + 2); // Second triangle
        indices.push_back(offset + 3);
        indices.push_back(offset + 0);

        offset += 4;  // Move to the next set of 4 vertices
    }

    // Create EBO with populated indices
    color_buffer_ebo = std::make_unique<EBO>(indices.data(), indices.size() * sizeof(GLuint));
    color_buffer_ebo->Bind();

    shader = colorBufferShader;
}

/*!***********************************************************************
\brief
Clears the vertex buffer and prepares for a new batch of debug boxes.
*************************************************************************/
void ColorBufferBatchRenderer2D::beginBatch()
{
    vertices.clear();
}

/*!***********************************************************************
\brief
Adds a debug box to the batch. The box is defined by its position, size,
color, and rotation.

\param position
The position of the box center in world coordinates.

\param size
The size (width and height) of the box.

\param color
The color of the box in RGB format.

\param rotation
The rotation angle of the box, in radians.
*************************************************************************/
void ColorBufferBatchRenderer2D::drawDebugBox(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, float rotation)
{
    if (vertices.size() >= maxBufferSprites * 4) 
    {
        flush();
        beginBatch();
    }

    glm::vec2 halfSize = size * 0.5f;

    // Define corners relative to the box center
    glm::vec2 corners[4] = {
        glm::vec2(-halfSize.x, -halfSize.y), // Bottom-left
        glm::vec2(halfSize.x, -halfSize.y),  // Bottom-right
        glm::vec2(halfSize.x, halfSize.y),   // Top-right
        glm::vec2(-halfSize.x, halfSize.y)   // Top-left
    };

    // Apply rotation and translation
    glm::vec2 rotatedCorners[4];
    float cosTheta = cos(rotation);
    float sinTheta = sin(rotation);

    for (int i = 0; i < 4; ++i) 
    {
        rotatedCorners[i] = glm::vec2(
            cosTheta * corners[i].x - sinTheta * corners[i].y,
            sinTheta * corners[i].x + cosTheta * corners[i].y
        ) + position;
    }

    // Add vertices
    vertices.push_back({ glm::vec3(rotatedCorners[0], 0.0f), color}); // Bottom-left
    vertices.push_back({ glm::vec3(rotatedCorners[1], 0.0f), color}); // Bottom-right
    vertices.push_back({ glm::vec3(rotatedCorners[2], 0.0f), color}); // Top-right
    vertices.push_back({ glm::vec3(rotatedCorners[3], 0.0f), color}); // Top-left
}

/*!***********************************************************************
\brief
Ends the current batch by rendering all the vertices in the buffer and
then deactivates the shader.
*************************************************************************/
void ColorBufferBatchRenderer2D::endBatch()
{
    flush();
    shader->Deactivate();
}

/*!***********************************************************************
\brief
Flushes the current batch, rendering all the debug boxes in the vertex
buffer using the bound shader and indices.
*************************************************************************/
void ColorBufferBatchRenderer2D::flush()
{
    if (vertices.empty()) return;

    color_buffer_vao->Bind(); 
    color_buffer_vbo->Bind(); 

    // Update vertex buffer with new data
    color_buffer_vbo->UpdateData(vertices.data(), vertices.size() * sizeof(ColorBufferVertex)); 

    // Bind the shader
    shader->Activate(); 

    // Draw all elements
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertices.size() / 4 * 6), GL_UNSIGNED_INT, 0);

    // Unbind VAO
    color_buffer_vao->Unbind(); 
    vertices.clear();
}
