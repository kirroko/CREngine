/*!***********************************************************************
\file       DebugModeBatchRendering.cpp
\author     Tan Shun Zhi Tomy, t.shunzhitomy@digipen.edu
\date       26/11/2024
\brief
This file contains the implementation of the `DebugBatchRenderer2D`
class, which facilitates batch rendering of debug wireframe boxes
using OpenGL.

The renderer is optimized for visualizing debug wireframes by batching
vertices for multiple boxes and rendering them in a single draw call.
It uses `GL_LINES` for drawing individual edges, and supports
rotation and scaling of each debug box.

\details
- The `init` function sets up the VAO and VBO for managing vertex data.
- The `drawDebugBox` function adds the vertices for a single box into
  the current batch.
- The `flush` function renders the batch of debug boxes in the buffer
  and resets the batch for the next set of debug boxes.

This renderer is primarily used in debug modes to visualize bounding
boxes, collision shapes, or other debug information.

\copyright
Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*************************************************************************/
#include "PreCompile.h"
#include "DebugModeBatchRendering.h"
#include "shaderClass.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"
#include "Texture.h"

DebugBatchRenderer2D::DebugBatchRenderer2D()
{
}

/*!***********************************************************************
\brief
Destructor for the `DebugBatchRenderer2D` object. Cleans up resources
such as VAO, VBO, and clears the vertex buffer.
*************************************************************************/
DebugBatchRenderer2D::~DebugBatchRenderer2D()
{
    if (debug_vao) debug_vao->Delete();
    if (debug_vbo) debug_vbo->Delete();
    vertices.clear();
}

/*!***********************************************************************
\brief
Initializes the batch renderer with a shader and sets up the necessary
vertex buffer and vertex array object (VAO).

\param debugShader
The shader to be used for rendering debug wireframes.
*************************************************************************/
void DebugBatchRenderer2D::init(std::shared_ptr<Shader> debugShader)
{
    vertices.reserve(maxShapes * 4);
    debug_vao = std::make_unique<VAO>();
    debug_vao->Bind();

    // Create VBO for vertices
    debug_vbo = std::make_unique<VBO>(nullptr, sizeof(debugVertex) * maxShapes * 4);
    debug_vbo->Bind();

    // Link vertex attributes (position)
    debug_vao->LinkAttrib(*debug_vbo, 0, 3, GL_FLOAT, sizeof(debugVertex), (void*)offsetof(debugVertex, position));
    debug_vao->LinkAttrib(*debug_vbo, 1, 3, GL_FLOAT, sizeof(debugVertex), (void*)offsetof(debugVertex, color));
    // No EBO is needed for GL_LINE_LOOP, but you can still use one for polygons
    debug_vao->Unbind();
    debug_vbo->Unbind();

    shader = debugShader;  // Assign the debug shader
}

/*!***********************************************************************
\brief
Clears the vertex buffer and prepares for a new batch of debug wireframe
boxes.
*************************************************************************/
void DebugBatchRenderer2D::beginBatch()
{
    vertices.clear();
}

/*!***********************************************************************
\brief
Adds a debug wireframe box to the batch. The box is defined by its
position, size, and rotation.

\param position
The position of the box center in world coordinates.

\param size
The size (width and height) of the box.

\param rotation
The rotation angle of the box, in radians.
*************************************************************************/
void DebugBatchRenderer2D::drawDebugBox(const glm::vec3& position, const glm::vec2& size, const glm::vec3& color, float rotation)
{
    if (vertices.size() >= maxShapes * 4) {
        flush();
        beginBatch();
    }

    // Calculate half-size
    glm::vec2 halfSize = size * 0.5f;

    // Define the corners of the box
    glm::vec2 corners[4] = {
        glm::vec2(-halfSize.x, -halfSize.y), // Bottom-left
        glm::vec2(halfSize.x, -halfSize.y),  // Bottom-right
        glm::vec2(halfSize.x, halfSize.y),   // Top-right
        glm::vec2(-halfSize.x, halfSize.y)   // Top-left
    };

    // Rotate and translate corners
    glm::vec2 rotatedCorners[4];
    float cosTheta = cos(rotation);
    float sinTheta = sin(rotation);

    for (int i = 0; i < 4; ++i) {
        rotatedCorners[i] = glm::vec2(
            cosTheta * corners[i].x - sinTheta * corners[i].y,
            sinTheta * corners[i].x + cosTheta * corners[i].y
        );
    }

    // Push vertices to the batch with Z-axis support
    vertices.push_back({ glm::vec3(rotatedCorners[0].x + position.x, rotatedCorners[0].y + position.y, position.z), color }); // Bottom-left -> Bottom-right
    vertices.push_back({ glm::vec3(rotatedCorners[1].x + position.x, rotatedCorners[1].y + position.y, position.z), color });

    vertices.push_back({ glm::vec3(rotatedCorners[1].x + position.x, rotatedCorners[1].y + position.y, position.z), color }); // Bottom-right -> Top-right
    vertices.push_back({ glm::vec3(rotatedCorners[2].x + position.x, rotatedCorners[2].y + position.y, position.z), color });

    vertices.push_back({ glm::vec3(rotatedCorners[2].x + position.x, rotatedCorners[2].y + position.y, position.z), color }); // Top-right -> Top-left
    vertices.push_back({ glm::vec3(rotatedCorners[3].x + position.x, rotatedCorners[3].y + position.y, position.z), color });

    vertices.push_back({ glm::vec3(rotatedCorners[3].x + position.x, rotatedCorners[3].y + position.y, position.z), color }); // Top-left -> Bottom-left
    vertices.push_back({ glm::vec3(rotatedCorners[0].x + position.x, rotatedCorners[0].y + position.y, position.z), color });
}

void DebugBatchRenderer2D::drawDebugCircle(const glm::vec2& center, float radius, const glm::vec3& color, int segments)
{
    if (vertices.size() + segments * 2 >= maxShapes * 4) {
        flush();
        beginBatch();
    }

    float angleStep = 2.0f * 3.14159265359f / segments;
    glm::vec2 prevPoint = center + glm::vec2(cos(0.0f), sin(0.0f)) * radius;

    for (int i = 1; i <= segments; ++i) {
        float angle = i * angleStep;
        glm::vec2 currentPoint = center + glm::vec2(cos(angle), sin(angle)) * radius;

        // Add line segment
        vertices.push_back({ glm::vec3(prevPoint, 0.0f), color });
        vertices.push_back({ glm::vec3(currentPoint, 0.0f), color });

        prevPoint = currentPoint;
    }
}

void DebugBatchRenderer2D::drawDebugLine(const glm::vec2& start, const glm::vec2& end, const glm::vec3& color)
{
    if (vertices.size() >= maxShapes * 2) // Each line has 2 vertices
    {
        flush();
        beginBatch();
    }

    // Add the start and end points of the line
    vertices.push_back({ glm::vec3(start, 0.0f), color });
    vertices.push_back({ glm::vec3(end, 0.0f), color });
}


/*!***********************************************************************
\brief
Ends the current batch by rendering all the vertices in the buffer.
*************************************************************************/
void DebugBatchRenderer2D::endBatch()
{
    flush();
}

/*!***********************************************************************
\brief
Flushes the current batch, rendering all the debug wireframe boxes in
the vertex buffer using the bound shader.
*************************************************************************/
void DebugBatchRenderer2D::flush()
{
    if (vertices.empty()) return;

    debug_vao->Bind();
    debug_vbo->Bind();

    shader->Activate();

    // Update vertex buffer with new data
    debug_vbo->UpdateData(vertices.data(), vertices.size() * sizeof(debugVertex));

    glLineWidth(2.0f);

    //shader->setVec3("color", glm::vec3(1.f, 0.f, 0.f));

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));

    debug_vao->Unbind();
    vertices.clear();  // Clear for the next batch
    shader->Deactivate();
}
