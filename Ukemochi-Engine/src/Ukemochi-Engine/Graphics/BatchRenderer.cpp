/*!
 * @file    BatchRenderer.cpp
 * @brief   Implementation of the BatchRenderer2D class for efficient sprite rendering.
 * @author  t.shunzhitomy@digipen.edu
 * @date    06/11/2024
 */
#include "PreCompile.h"
#include "BatchRenderer.h"
#include "shaderClass.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"
#include "Texture.h"

 /*!
  * @brief Constructs a new BatchRenderer2D object.
  */
BatchRenderer2D::BatchRenderer2D(){
    // Initialize batch renderer
}

/*!
 * @brief Destroys the BatchRenderer2D object and cleans up resources.
 */
BatchRenderer2D::~BatchRenderer2D() {
    // Cleanup VAO, VBO, and EBO
    vao->Delete();
    vbo->Delete();
    ebo->Delete();
    vertices.clear();
}

/*!
 * @brief Initializes the batch renderer with a shared shader and sets up vertex and index buffers.
 * @param sharedShader The shader to be used for rendering.
 */
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
}

/*!
 * @brief Clears the vertex buffer to start a new batch.
 */
void BatchRenderer2D::beginBatch()
{
    vertices.clear();
}

/*!
 * @brief Ends the current batch by flushing it for rendering.
 */
void BatchRenderer2D::endBatch()
{
    flush();
}

/*!
 * @brief Creates and configures the VAO for the batch.
 */
void BatchRenderer2D::createVertexArray() 
{
    vao = std::make_unique<VAO>();
    vao->Bind();
}

/*!
 * @brief Creates and configures the VBO for the batch and links vertex attributes.
 */
void BatchRenderer2D::createVertexBuffer()
{
    vbo = std::make_unique<VBO>(nullptr, sizeof(Vertex) * maxSprites * 4);
    vbo->Bind();

    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position)); 
    vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, color)); 
    vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoord)); 
    vao->LinkAttribInteger(*vbo, 3, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, textureID));
}

/*!
 * @brief Adds a sprite to the batch, handling rotation and texture information.
 * @param position Position of the sprite.
 * @param size Size of the sprite.
 * @param color Color tint of the sprite.
 * @param textureID Texture ID to use.
 * @param uvCoordinates UV coordinates for the texture.
 * @param rotation Rotation angle in radians.
 */
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

}

/*!
 * @brief Flushes the batch, rendering all sprites in the vertex buffer.
 */
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

    // Calculate the correct index count based on the number of quads in the batch
    int indexCount = static_cast<int>((vertices.size() / 4) * 6); // Each quad has 6 indices

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    vao->Unbind();
    ebo->Unbind();

    vertices.clear();
}

