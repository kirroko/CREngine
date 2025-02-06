/* Start Header ************************************************************************/
/*!
\file       BatchRenderer.cpp
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Nov 6, 2024
\brief      Implementation of the BatchRenderer2D class for efficient sprite rendering.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "../ECS/ECS.h"
#include "../Asset Manager/AssetManager.h"
#include "BatchRenderer.h"
#include "shaderClass.h"
#include "EBO.h"
#include "VBO.h"
#include "VAO.h"
#include "Texture.h"


 /*!
  * @brief Constructs a new BatchRenderer2D object.
  */
BatchRenderer2D::BatchRenderer2D() {
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
void BatchRenderer2D::drawSprite(const glm::vec3& position, const glm::vec2& size, const glm::vec3& color, GLint textureID, const GLfloat* uvCoordinates, float rotation)
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
    glm::vec3 pos1 = glm::vec3(bottomLeft + glm::vec2(position.x, position.y), position.z); // Bottom-left
    glm::vec3 pos2 = glm::vec3(bottomRight + glm::vec2(position.x, position.y), position.z); // Bottom-right
    glm::vec3 pos3 = glm::vec3(topRight + glm::vec2(position.x, position.y), position.z); // Top-right
    glm::vec3 pos4 = glm::vec3(topLeft + glm::vec2(position.x, position.y), position.z); // Top-left

    // Push vertices with updated UV coordinates and texture ID
    vertices.push_back({ pos1, color, {uvCoordinates[0], uvCoordinates[1]}, textureID });
    vertices.push_back({ pos2, color, {uvCoordinates[2], uvCoordinates[3]}, textureID });
    vertices.push_back({ pos3, color, {uvCoordinates[4], uvCoordinates[5]}, textureID });
    vertices.push_back({ pos4, color, {uvCoordinates[6], uvCoordinates[7]}, textureID });
}

void BatchRenderer2D::drawSprite(const glm::vec3& position, const glm::vec2& size, const glm::vec3& color, GLint textureID, const GLfloat* uvCoordinates, float rotation, int layer)
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
    glm::vec3 pos1 = glm::vec3(bottomLeft + glm::vec2(position.x, position.y), position.z); // Bottom-left
    glm::vec3 pos2 = glm::vec3(bottomRight + glm::vec2(position.x, position.y), position.z); // Bottom-right
    glm::vec3 pos3 = glm::vec3(topRight + glm::vec2(position.x, position.y), position.z); // Top-right
    glm::vec3 pos4 = glm::vec3(topLeft + glm::vec2(position.x, position.y), position.z); // Top-left

    // Create vertices
    Vertex v1 = { pos1, color, {uvCoordinates[0], uvCoordinates[1]}, textureID };
    Vertex v2 = { pos2, color, {uvCoordinates[2], uvCoordinates[3]}, textureID };
    Vertex v3 = { pos3, color, {uvCoordinates[4], uvCoordinates[5]}, textureID };
    Vertex v4 = { pos4, color, {uvCoordinates[6], uvCoordinates[7]}, textureID };

    // Add vertices to the appropriate layer
    layerBatches[layer].push_back(v1);
    layerBatches[layer].push_back(v2);
    layerBatches[layer].push_back(v3);
    layerBatches[layer].push_back(v4);
}

void BatchRenderer2D::drawSprite(const glm::vec3& position, const glm::vec2& size, const glm::vec3& color, const std::string& spriteName, float rotation, int layer)
{
    // Check if the batch is full and flush it
    if (vertices.size() >= maxSprites * 4)
    {
        std::cout << "Reached maxSprites in batch, flushing..." << std::endl;
        flush();
        beginBatch();
    }

    GLint textureID = -1;

    auto assetManager = ECS::GetInstance().GetSystem<AssetManager>();

    //std::cout << "Checking spriteData for spriteName: " << spriteName << std::endl;
    //if (assetManager->spriteData.find(spriteName) == assetManager->spriteData.end())
    //{
    //    std::cerr << "Sprite '" << spriteName << "' not found in spriteData." << std::endl;

    //    // Print all keys in spriteData for verification
    //    for (const auto& [key, value] : assetManager->spriteData)
    //    {
    //        std::cout << "Available key in spriteData: " << key << std::endl;
    //    }
    //    return;
    //}


    // Check if the sprite exists in the atlas
    if (assetManager->spriteData.find(spriteName) == assetManager->spriteData.end())
    {
        // Handle standalone texture
        auto texture = assetManager->getTexture(spriteName);
        if (!texture)
        {
            std::cerr << "Error: Sprite '" << spriteName << "' not found in atlas or as a standalone texture!" << std::endl;
            return;
        }

        // Bind the standalone texture
        texture->Bind();

        // Retrieve the texture ID
        textureID = ECS::GetInstance().GetSystem<Renderer>()->textureIDMap[texture->ID];

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
        glm::vec3 pos1 = glm::vec3(bottomLeft + glm::vec2(position.x, position.y), position.z); // Bottom-left
        glm::vec3 pos2 = glm::vec3(bottomRight + glm::vec2(position.x, position.y), position.z); // Bottom-right
        glm::vec3 pos3 = glm::vec3(topRight + glm::vec2(position.x, position.y), position.z); // Top-right
        glm::vec3 pos4 = glm::vec3(topLeft + glm::vec2(position.x, position.y), position.z); // Top-left

        // Define proper UV coordinates for standalone textures
        glm::vec2 uv1 = { 0.0f, 0.0f }; // Bottom-left
        glm::vec2 uv2 = { 1.0f, 0.0f }; // Bottom-right
        glm::vec2 uv3 = { 1.0f, 1.0f }; // Top-right
        glm::vec2 uv4 = { 0.0f, 1.0f }; // Top-left

        // Create vertices
        Vertex v1 = { pos1, color, {uv1}, textureID };
        Vertex v2 = { pos2, color, {uv2}, textureID };
        Vertex v3 = { pos3, color, {uv3}, textureID };
        Vertex v4 = { pos4, color, {uv4}, textureID };

        // Add vertices to the appropriate layer
        layerBatches[layer].push_back(v1);
        layerBatches[layer].push_back(v2);
        layerBatches[layer].push_back(v3);
        layerBatches[layer].push_back(v4);

        return;
    }


    //ECS::GetInstance().GetSystem<AssetManager>()->debugPrintSpriteData();
    // Retrieve UV coordinates for the sprite from AssetManager
    const auto& spriteInfo = ECS::GetInstance().GetSystem<AssetManager>()->getSpriteData(spriteName);
    const auto& uv = spriteInfo.uv;

    // May not be here
    ECS::GetInstance().GetSystem<AssetManager>()->bindSpriteSheet(spriteInfo.spriteSheetName);

    // Retrieve the texture ID of the atlas
    auto atlasTexture = assetManager->getTexture(spriteInfo.spriteSheetName);
    if (!atlasTexture)
    {
        std::cerr << "Error: Atlas texture '" << spriteInfo.spriteSheetName << "' not found!" << std::endl;
        return;
    }

    textureID = ECS::GetInstance().GetSystem<Renderer>()->textureIDMap[atlasTexture->ID];

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
    glm::vec3 pos1 = glm::vec3(bottomLeft + glm::vec2(position.x, position.y), position.z); // Bottom-left
    glm::vec3 pos2 = glm::vec3(bottomRight + glm::vec2(position.x, position.y), position.z); // Bottom-right
    glm::vec3 pos3 = glm::vec3(topRight + glm::vec2(position.x, position.y), position.z); // Top-right
    glm::vec3 pos4 = glm::vec3(topLeft + glm::vec2(position.x, position.y), position.z); // Top-left

    // Create vertices with UV coordinates from the atlas
    Vertex v1 = { pos1, color, {uv.uMin, uv.vMin}, textureID };
    Vertex v2 = { pos2, color, {uv.uMax, uv.vMin}, textureID };
    Vertex v3 = { pos3, color, {uv.uMax, uv.vMax}, textureID };
    Vertex v4 = { pos4, color, {uv.uMin, uv.vMax}, textureID };

    // Add vertices to the appropriate layer
    layerBatches[layer].push_back(v1);
    layerBatches[layer].push_back(v2);
    layerBatches[layer].push_back(v3);
    layerBatches[layer].push_back(v4);
}


void BatchRenderer2D::flush()
{
    std::vector<int> layers;
    for (const auto& entry : layerBatches)
        layers.push_back(entry.first);

    std::sort(layers.begin(), layers.end()); // Ensure lower layers render first

    /*std::cout << "Rendering UI Layers in Order: ";
    for (int layer : layers)
        std::cout << layer << " ";
    std::cout << std::endl;*/

    for (int layer : layers)
    {
        auto& layerVertices = layerBatches[layer];
        if (layerVertices.empty()) continue;

        //std::cout << "Layer " << layer << " has " << layerVertices.size() << " vertices\n";

        vao->Bind();
        vbo->Bind();
        vbo->UpdateData(layerVertices.data(), layerVertices.size() * sizeof(Vertex));
        vbo->Unbind();
        ebo->Bind();

        if (activeShader)
        {
            activeShader->Activate();
        }

        int indexCount = static_cast<int>((layerVertices.size() / 4) * 6);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

        vao->Unbind();
    }

    layerBatches.clear();
}

void BatchRenderer2D::setActiveShader(std::shared_ptr<Shader> ashader)
{
    activeShader = ashader; // Assign the new shader
}