#include "PreCompile.h"
#include "ColorBufferBatchRendering.h"
#include "shaderClass.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"
#include "Texture.h"

ColorBufferBatchRenderer2D::ColorBufferBatchRenderer2D()
{
}

ColorBufferBatchRenderer2D::~ColorBufferBatchRenderer2D()
{
	if (color_buffer_vao) color_buffer_vao->Delete();
	if (color_buffer_vbo) color_buffer_vbo->Delete();
	if (color_buffer_ebo) color_buffer_ebo->Delete();
	indices.clear();
	vertices.clear();
}

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
    for (int i = 0; i < maxBufferSprites; ++i) {
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

    shader = colorBufferShader; // Assign shader
}

void ColorBufferBatchRenderer2D::beginBatch()
{
    vertices.clear();
}

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

    for (int i = 0; i < 4; ++i) {
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

void ColorBufferBatchRenderer2D::endBatch()
{
    flush();
    shader->Deactivate();
}

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
