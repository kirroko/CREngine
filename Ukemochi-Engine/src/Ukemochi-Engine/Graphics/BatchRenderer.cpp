#include "PreCompile.h"
#include "BatchRenderer.h"
#include "shaderClass.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"
#include "Texture.h"

BatchRenderer2D::BatchRenderer2D(){
    // Initialize batch renderer
}

//BatchRenderer2D::BatchRenderer2D() {
//    // Constructor
//}

BatchRenderer2D::~BatchRenderer2D() {
    // Cleanup VAO, VBO, and EBO
    vao->Delete();
    vbo->Delete();
    ebo->Delete();
}

void BatchRenderer2D::init(std::shared_ptr<Shader> sharedShader) 
{
    // Reserve memory for vertices and indices
    vertices.reserve(maxSprites * 4); // 4 vertices per sprite
    indices.reserve(maxSprites * 6);  // 6 indices per sprite (two triangles)

    // Create the VAO and bind it
    createVertexArray();

    // Create the VBO, bind it, and link vertex attributes
    createVertexBuffer();

    // Populate the `indices` array with correct offsets
    int offset = 0;
    for (int i = 0; i < maxSprites; ++i) {
        indices.push_back(offset + 0); // First triangle
        indices.push_back(offset + 1);
        indices.push_back(offset + 2);

        indices.push_back(offset + 2); // Second triangle
        indices.push_back(offset + 3);
        indices.push_back(offset + 0);

        offset += 4;  // Move to the next set of 4 vertices for the next sprite
    }

    // Create EBO with populated indices
    ebo = std::make_unique<EBO>(indices.data(), indices.size() * sizeof(GLuint));
    ebo->Bind();

    // Unbind VAO, VBO, and EBO to avoid unintended modification
    vao->Unbind();
    vbo->Unbind();
    ebo->Unbind();

    shader = sharedShader;
    std::cout << "BatchRenderer initialized with maxSprites: " << maxSprites << std::endl;
}

void BatchRenderer2D::beginBatch()
{
    vertices.clear();
}

void BatchRenderer2D::endBatch()
{
    flush();
}

void BatchRenderer2D::createVertexArray() 
{
    vao = std::make_unique<VAO>();
    vao->Bind();
}

void BatchRenderer2D::createVertexBuffer()
{
    vbo = std::make_unique<VBO>(nullptr, sizeof(Vertex) * maxSprites * 4);
    vbo->Bind();

    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position)); 
    vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
    vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoord)); 
    vao->LinkAttribInteger(*vbo, 3, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, textureID));
}

void BatchRenderer2D::drawSprite(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, GLint textureID, const GLfloat* uvCoordinates, float rotation)
{
    if (vertices.size() >= maxSprites * 4)
    {
        std::cout << "Reached maxSprites in batch, flushing..." << std::endl;
        flush();
        beginBatch();
    }

    // Calculate sine and cosine for the rotation angle
    float cosTheta = cos(rotation);
    float sinTheta = sin(rotation);

    // Define the four corners of the sprite relative to its center
    glm::vec2 halfSize = size * 0.5f;

    glm::vec2 bottomLeft(-halfSize.x, -halfSize.y);
    glm::vec2 bottomRight(halfSize.x, -halfSize.y);
    glm::vec2 topRight(halfSize.x, halfSize.y);
    glm::vec2 topLeft(-halfSize.x, halfSize.y);

    // Apply rotation to each corner
    bottomLeft = glm::vec2(
        cosTheta * bottomLeft.x - sinTheta * bottomLeft.y,
        sinTheta * bottomLeft.x + cosTheta * bottomLeft.y
    );

    bottomRight = glm::vec2(
        cosTheta * bottomRight.x - sinTheta * bottomRight.y,
        sinTheta * bottomRight.x + cosTheta * bottomRight.y
    );

    topRight = glm::vec2(
        cosTheta * topRight.x - sinTheta * topRight.y,
        sinTheta * topRight.x + cosTheta * topRight.y
    );

    topLeft = glm::vec2(
        cosTheta * topLeft.x - sinTheta * topLeft.y,
        sinTheta * topLeft.x + cosTheta * topLeft.y
    );

    // Translate rotated vertices to the actual position of the sprite
    glm::vec3 pos1 = glm::vec3(bottomLeft + position, 0.0f); // Bottom-left
    glm::vec3 pos2 = glm::vec3(bottomRight + position, 0.0f); // Bottom-right
    glm::vec3 pos3 = glm::vec3(topRight + position, 0.0f); // Top-right
    glm::vec3 pos4 = glm::vec3(topLeft + position, 0.0f); // Top-left

    // Push vertices with updated UV coordinates and texture ID
    vertices.push_back({ pos1, color, {uvCoordinates[0], uvCoordinates[1]}, textureID });
    vertices.push_back({ pos2, color, {uvCoordinates[2], uvCoordinates[3]}, textureID });
    vertices.push_back({ pos3, color, {uvCoordinates[4], uvCoordinates[5]}, textureID });
    vertices.push_back({ pos4, color, {uvCoordinates[6], uvCoordinates[7]}, textureID });

   // std::cout << "Sprite rotation angle: " << rotation << " radians" << std::endl;
}

//void BatchRenderer2D::drawSprite(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color)
//{
//    // Check if we need to flush the batch
//    if (vertices.size() >= maxSprites * 4) 
//    {
//        std::cout << "Reached maxSprites in batch, flushing..." << std::endl;
//        flush();
//        beginBatch();
//    }
//
//    // Calculate the positions of the four corners of the sprite based on the position and size
//   // Assuming the center of the sprite is at `position`
//    glm::vec3 pos1 = glm::vec3(position.x - size.x / 2.0f, position.y - size.y / 2.0f, 0.0f); // Bottom-left
//    glm::vec3 pos2 = glm::vec3(position.x + size.x / 2.0f, position.y - size.y / 2.0f, 0.0f); // Bottom-right
//    glm::vec3 pos3 = glm::vec3(position.x + size.x / 2.0f, position.y + size.y / 2.0f, 0.0f); // Top-right
//    glm::vec3 pos4 = glm::vec3(position.x - size.x / 2.0f, position.y + size.y / 2.0f, 0.0f); // Top-left
//
//    // Define UV coordinates for a basic texture mapping (if textures are involved)
//    glm::vec2 uv1(0.0f, 0.0f); // Bottom-left UV
//    glm::vec2 uv2(1.0f, 0.0f); // Bottom-right UV
//    glm::vec2 uv3(1.0f, 1.0f); // Top-right UV
//    glm::vec2 uv4(0.0f, 1.0f); // Top-left UV
//
//    // Push vertices into the batch's vertex buffer
//    vertices.push_back({ pos1, color, uv1 });
//    vertices.push_back({ pos2, color, uv2 });
//    vertices.push_back({ pos3, color, uv3 });
//    vertices.push_back({ pos4, color, uv4 });
//
//}


void BatchRenderer2D::flush() 
{
    if (vertices.empty()) 
    {
        //std::cout << "No vertices to flush." << std::endl;
        return;
    }

    // Bind VAO and Shader
    vao->Bind();

    // Update VBO data with current vertices
    vbo->Bind();

    // Print vertex data for debugging
    vbo->UpdateData(vertices.data(), vertices.size() * sizeof(Vertex));
    vbo->Unbind(); 

    // Bind EBO
    ebo->Bind();
    shader->Activate();

    //std::cout << "Flushing batch with " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;
    // Calculate the correct index count based on the number of quads in the batch
    int indexCount = (vertices.size() / 4) * 6; // Each quad has 6 indices

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    vao->Unbind();
    ebo->Unbind();

    vertices.clear();
   // std::cout << "Batch cleared after flush." << std::endl;

}

