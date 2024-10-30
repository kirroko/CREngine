#include "PreCompile.h"
#include "BatchRenderer.h"
#include "shaderClass.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"

BatchRenderer2D::BatchRenderer2D() {
    // Constructor
}

BatchRenderer2D::~BatchRenderer2D() {
    // Cleanup VAO, VBO, and EBO
    vao->Delete();
    vbo->Delete();
    ebo->Delete();
}

void BatchRenderer2D::init(std::shared_ptr<Shader> sharedShader) {
    // Reserve memory and create VAO, VBO, EBO
    vertices.reserve(maxSprites * 4);
    indices.reserve(maxSprites * 6);
    
    // Create unique pointers for VAO, VBO, and EBO
    vao = std::make_unique<VAO>();
    vbo = std::make_unique<VBO>(nullptr, sizeof(Vertex) * maxSprites * 4);

    createVertexArray();
    createVertexBuffer();
    createIndexBuffer();

    shader = sharedShader;
}

void BatchRenderer2D::beginBatch()
{
    vertices.clear();
    indices.clear();
}

void BatchRenderer2D::endBatch()
{
    flush();
}

void BatchRenderer2D::createVertexArray() 
{
    vao->Bind();
}

void BatchRenderer2D::createVertexBuffer() 
{
    vbo = std::make_unique<VBO>(nullptr, sizeof(Vertex) * maxSprites * 4);
    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
    vao->LinkAttrib(*vbo, 1, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    vao->LinkAttrib(*vbo, 2, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));
}

void BatchRenderer2D::createIndexBuffer() {
    std::vector<unsigned int> indices;
    indices.resize(maxSprites * 6);
    int offset = 0;
    for (int i = 0; i < maxSprites; i++) {
        indices[i * 6 + 0] = offset + 0;
        indices[i * 6 + 1] = offset + 1;
        indices[i * 6 + 2] = offset + 2;
        indices[i * 6 + 3] = offset + 2;
        indices[i * 6 + 4] = offset + 3;
        indices[i * 6 + 5] = offset + 0;
        offset += 4;
    }
    ebo = std::make_unique<EBO>(&indices[0], indices.size() * sizeof(unsigned int));
}

void BatchRenderer2D::drawSprite(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color, int textureID) {
    // Push vertices to the buffer
    if (vertices.size() >= maxSprites * 4) {
        flush();
        beginBatch();
    }

    glm::vec3 pos1 = glm::vec3(position, 0.0f);
    glm::vec3 pos2 = glm::vec3(position.x + size.x, position.y, 0.0f);
    glm::vec3 pos3 = glm::vec3(position.x + size.x, position.y + size.y, 0.0f);
    glm::vec3 pos4 = glm::vec3(position.x, position.y + size.y, 0.0f);

    vertices.push_back({ pos1, glm::vec2(0.0f, 0.0f), color });
    vertices.push_back({ pos2, glm::vec2(1.0f, 0.0f), color });
    vertices.push_back({ pos3, glm::vec2(1.0f, 1.0f), color });
    vertices.push_back({ pos4, glm::vec2(0.0f, 1.0f), color });
}

void BatchRenderer2D::flush() 
{
    if (vertices.empty()) return;

    vao->Bind();
    shader->Activate();

    vbo->UpdateData(vertices.data(), vertices.size() * sizeof(Vertex));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    vertices.clear();
}
