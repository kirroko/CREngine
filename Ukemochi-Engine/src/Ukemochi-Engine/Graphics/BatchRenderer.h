/* Start Header ************************************************************************/
/*!
\file       BatchRenderer.h
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Nov 6, 2024
\brief      Header file for the BatchRenderer2D class, which handles efficient rendering of multiple 2D sprites in batches.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef BATCH_RENDERER_H
#define BATHC_RENDERER_H

#include "PreCompile.h" 
#include "../vendor/glm/glm/glm.hpp"
#include "GLAD/glad.h"

// Forward declaration
class VAO;
class VBO;
class EBO;
class Shader;
class Texture;

/*!
 * @struct Vertex
 * @brief  Represents a vertex with position, color, texture coordinates, and a texture ID.
 */
struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;
	GLint textureID;
};

/*!
 * @class BatchRenderer2D
 * @brief A class to manage batch rendering of 2D sprites for improved performance.
 */
class BatchRenderer2D {
public:
	/*!
	 * @brief Constructs a new BatchRenderer2D object.
	 */
	BatchRenderer2D();
	/*!
	 * @brief Destructor for cleaning up resources used by the BatchRenderer2D.
	 */
	~BatchRenderer2D();

	/*!
	 * @brief Initializes the renderer with a shared shader.
	 * @param sharedShader Shared pointer to the shader used for rendering.
	 */
	void init(std::shared_ptr<Shader> sharedShader);
	/*!
	* @brief Begins a new batch by clearing the vertex buffer.
	*/
	void beginBatch();
	/*!
	 * @brief Ends the current batch and flushes it for rendering.
	 */
	void endBatch();
	/*!
	 * @brief Renders all sprites in the current batch.
	 */
	void flush();
	/*!
	 * @brief Adds a sprite to the batch with rotation, texture, and color.
	 * @param position The position of the sprite.
	 * @param size The size of the sprite.
	 * @param color The color tint to apply to the sprite.
	 * @param textureID The ID of the texture to use.
	 * @param uvCoordinates The UV coordinates for the texture.
	 * @param rotation The rotation angle for the sprite.
	 */
	void drawSprite(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, GLint textureID, const GLfloat* uvCoordinates, float rotation = 0.0f);

	void setActiveShader(std::shared_ptr<Shader> shader); // Update method signature

	std::vector<Vertex>& getVertices() { return vertices; } // Getter for vertices
private:
	/*!
	 * @brief Creates and configures the Vertex Array Object (VAO) for the batch.
	 */
	void createVertexArray();
	/*!
	 * @brief Creates and configures the Vertex Buffer Object (VBO) for the batch.
	 */
	void createVertexBuffer();

	std::unique_ptr<VAO> vao; // Vertex Array Object
	std::unique_ptr<VBO> vbo; // Vertex Buffer Object
	std::unique_ptr<EBO> ebo; // Element Buffer Object
	std::shared_ptr<Shader> shader; // Shader used for rendering
	std::vector<Vertex> vertices; // Vertex buffer for batching
	std::vector<GLuint> indices; // Index buffer for batching

	int maxSprites = 1000; // Maximum number of sprites per batch

	std::shared_ptr<Shader> activeShader; // Use shared_ptr for the active shader 
};

#endif // !BATCH_RENDERER_H

