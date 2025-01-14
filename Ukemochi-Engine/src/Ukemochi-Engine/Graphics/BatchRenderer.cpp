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
    queryMaxTextureUnits();

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
 * @brief Checks the current texture limit of the hardware and ensures that it is less
 *        than or equals to 32
 */
void BatchRenderer2D::queryMaxTextureUnits()
{
    GLint maxTextureUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits); // Get the texture limit of the system
    maxTexturesPerPass = std::min(32, maxTextureUnits); // Caps it to be 32 but can be lesser as well
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
    // Finds textureID being used for the sprite is already in the map
    // If its not inside, add it in the map else it skips this statement
    if (textureUnitMap.find(textureID) == textureUnitMap.end())
    {
        // If the number of textures exceeds the systems texture limit it will flush everything to free up space
        if (activeTextures.size() >= maxTexturesPerPass)
        {
            flush();
            beginBatch();
        }

        // Assigns the textureID to a texture unit index
        // activeTextures.size() is a easy way to increment the index
        // e.g. if the size of the vector is currently 0, it will mean that
        // activeTextures.size() will be 0, hence index will be 0.
        // Now that the vector has one value and its size is 1, activeTextures.size()
        // will be 1 and hence for the next textureID i will have a index value of 1
        textureUnitMap[textureID] = static_cast<int>(activeTextures.size());
        
        // Adds the textureID to activeTextures to keep track of which textures are being used
        activeTextures.push_back(textureID);
    }

    int textureUnit = textureUnitMap[textureID];

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
    Vertex v1 = { pos1, color, {uvCoordinates[0], uvCoordinates[1]}, textureUnit };
    Vertex v2 = { pos2, color, {uvCoordinates[2], uvCoordinates[3]}, textureUnit };
    Vertex v3 = { pos3, color, {uvCoordinates[4], uvCoordinates[5]}, textureUnit };
    Vertex v4 = { pos4, color, {uvCoordinates[6], uvCoordinates[7]}, textureUnit };

    // Add vertices to the appropriate layer
    layerBatches[layer].push_back(v1);
    layerBatches[layer].push_back(v2);
    layerBatches[layer].push_back(v3);
    layerBatches[layer].push_back(v4);
}

//void BatchRenderer2D::flush()
//{
//    // layer and layerVertices represent the layer number e.g 1, 2, 3 and layerVertices represent 
//    // the vertices in each layer
//    for (const auto& [layer, layerVertices] : layerBatches) 
//    {
//        if (layerVertices.empty()) 
//            continue;
//
//        // Bind VAO and Shader
//        vao->Bind();
//
//        // Update VBO data with current vertices
//        vbo->Bind();
//
//        // Print vertex data for debugging
//        vbo->UpdateData(layerVertices.data(), layerVertices.size() * sizeof(Vertex));
//        vbo->Unbind();
//
//        // Bind EBO
//        ebo->Bind();
//
//        if (activeShader) 
//        {
//            activeShader->Activate(); // Use the active shader
//        }
//
//        int indexCount = static_cast<int>((layerVertices.size() / 4) * 6); // 6 indices per quad
//        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
//
//        vao->Unbind();
//    }
//
//    vao->Unbind();
//    ebo->Unbind();
//    // Clear batches for the next frame
//    layerBatches.clear();
//}
void BatchRenderer2D::flush()
{
    for (const auto& [layer, layerVertices] : layerBatches)
    {
        if (layerVertices.empty())
            continue;

        // Keep track of processed vertices and active textures
        size_t vertexCount = 0;

        // Process in texture batches
        while (vertexCount < layerVertices.size())
        {
            // Reset textures for the current pass
            textureUnitMap.clear();
            activeTextures.clear();

            // Bind VAO and Shader
            vao->Bind();
            vbo->Bind();

            // Prepare a sub-range of vertices for this pass
            std::vector<Vertex> passVertices;
            size_t passStart = vertexCount;
            size_t passEnd = layerVertices.size();

            for (size_t i = passStart; i < layerVertices.size(); ++i)
            {
                const auto& vertex = layerVertices[i];

                // Check if we need to bind a new texture
                if (textureUnitMap.find(vertex.textureID) == textureUnitMap.end())
                {
                    // If maxTexturesPerPass is reached, stop this pass
                    if (activeTextures.size() >= maxTexturesPerPass)
                    {
                        passEnd = i;
                        break;
                    }

                    // Bind the new texture and map it to a unit
                    textureUnitMap[vertex.textureID] = static_cast<int>(activeTextures.size());
                    activeTextures.push_back(vertex.textureID);
                }

                // Adjust the vertex's texture unit index
                passVertices.push_back(vertex);
                passVertices.back().textureID = textureUnitMap[vertex.textureID];
            }

            // Update the VBO with the vertices for this pass
            vbo->UpdateData(passVertices.data(), passVertices.size() * sizeof(Vertex));
            vbo->Unbind();

            // Bind the active textures
            for (int i = 0; i < activeTextures.size(); ++i)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, activeTextures[i]);
            }

            // Draw the elements for this pass
            if (activeShader)
            {
                activeShader->Activate();
            }

            int indexCount = static_cast<int>((passVertices.size() / 4) * 6); // 6 indices per quad
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(passStart * sizeof(GLuint)));

            vertexCount = passEnd;

            vao->Unbind();
        }
    }

    // Unbind EBO
    ebo->Unbind();

    // Clear texture state
    glActiveTexture(GL_TEXTURE0);
    activeTextures.clear();
    textureUnitMap.clear();

    // Clear batches for the next frame
    layerBatches.clear();
}

void BatchRenderer2D::setActiveShader(std::shared_ptr<Shader> ashader)
{
    activeShader = ashader; // Assign the new shader
}