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

DebugBatchRenderer2D::~DebugBatchRenderer2D()
{
    if (debug_vao) debug_vao->Delete();
    if (debug_vbo) debug_vbo->Delete();
    vertices.clear();
}

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

    // No EBO is needed for GL_LINE_LOOP, but you can still use one for polygons
    debug_vao->Unbind();
    debug_vbo->Unbind();

    shader = debugShader;  // Assign the debug shader
}

void DebugBatchRenderer2D::beginBatch()
{
    vertices.clear();
}

void DebugBatchRenderer2D::drawDebugBox(const glm::vec2& position, const glm::vec2& size, float rotation)
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
        ) + position;
    }

    // Push vertices to the batch
    // Add vertices for GL_LINES (2 vertices per line)
    vertices.push_back({ glm::vec3(rotatedCorners[0], 0.0f) }); // Bottom-left -> Bottom-right
    vertices.push_back({ glm::vec3(rotatedCorners[1], 0.0f) });

    vertices.push_back({ glm::vec3(rotatedCorners[1], 0.0f) }); // Bottom-right -> Top-right
    vertices.push_back({ glm::vec3(rotatedCorners[2], 0.0f) });

    vertices.push_back({ glm::vec3(rotatedCorners[2], 0.0f) }); // Top-right -> Top-left
    vertices.push_back({ glm::vec3(rotatedCorners[3], 0.0f) });

    vertices.push_back({ glm::vec3(rotatedCorners[3], 0.0f) }); // Top-left -> Bottom-left
    vertices.push_back({ glm::vec3(rotatedCorners[0], 0.0f) });
}

void DebugBatchRenderer2D::endBatch()
{
    flush();
}

void DebugBatchRenderer2D::flush()
{
    if (vertices.empty()) return;

    debug_vao->Bind();
    debug_vbo->Bind();

    shader->Activate();

    // Update vertex buffer with new data
    debug_vbo->UpdateData(vertices.data(), vertices.size() * sizeof(debugVertex));

    glLineWidth(2.0f);

    shader->setVec3("color", glm::vec3(1.f, 0.f, 0.f));

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));

    debug_vao->Unbind();
    vertices.clear();  // Clear for the next batch
    shader->Deactivate();
}
