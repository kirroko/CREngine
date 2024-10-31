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
    std::cout << "BatchRenderer initialized with maxSprites: " << maxSprites << std::endl;
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
    vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color));
    vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
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
    ebo = std::make_unique<EBO>(&indices[0], indices.size() * sizeof(GLuint));
    std::cout << "Generated indices:" << std::endl;
    for (size_t i = 0; i < indices.size(); i += 6) {
        std::cout << "Quad " << (i / 6) << ": "
            << indices[i] << ", " << indices[i + 1] << ", " << indices[i + 2] << ", "
            << indices[i + 3] << ", " << indices[i + 4] << ", " << indices[i + 5] << std::endl;
    }
}

void BatchRenderer2D::drawSprite(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color, int textureID) {
    // Push vertices to the buffer
    if (vertices.size() >= maxSprites * 4) {
        std::cout << "Reached maxSprites in batch, flushing..." << std::endl;
        flush();
        beginBatch();
    }

    glm::vec3 pos1 = glm::vec3(position, 0.0f);
    glm::vec3 pos2 = glm::vec3(position.x + size.x, position.y, 0.0f);
    glm::vec3 pos3 = glm::vec3(position.x + size.x, position.y + size.y, 0.0f);
    glm::vec3 pos4 = glm::vec3(position.x, position.y + size.y, 0.0f);

    // Extract RGB from the input color (ignoring the alpha channel)
    glm::vec3 rgbColor = glm::vec3(color.r, color.g, color.b);

    // Push vertices with the adjusted color
    vertices.push_back({ pos1, rgbColor, glm::vec2(0.0f, 0.0f) });
    vertices.push_back({ pos2, rgbColor, glm::vec2(1.0f, 0.0f) });
    vertices.push_back({ pos3, rgbColor, glm::vec2(1.0f, 1.0f) });
    vertices.push_back({ pos4, rgbColor, glm::vec2(0.0f, 1.0f) });
    std::cout << "Added sprite vertices for position: (" << position.x << ", " << position.y << ") and size: (" << size.x << ", " << size.y << ")" << std::endl;
}

void BatchRenderer2D::flush() 
{
    if (vertices.empty()) 
    {
        std::cout << "No vertices to flush." << std::endl;
        return;
    }

    vao->Bind();
    shader->Activate();

    vbo->UpdateData(vertices.data(), vertices.size() * sizeof(Vertex));
    
    ebo->Bind();

    std::cout << "Flushing batch with " << vertices.size() << " vertices and " << (indices.size()) << " indices." << std::endl;
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    std::cout << "Draw call issued with " << indices.size() << " indices." << std::endl;
    vertices.clear();
    std::cout << "Batch cleared after flush." << std::endl;
}
