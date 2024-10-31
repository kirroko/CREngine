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

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;
};


class BatchRenderer2D {
public:
	BatchRenderer2D();
	~BatchRenderer2D();

	void init(std::shared_ptr<Shader> sharedShader); 
	void beginBatch();
	void endBatch();
	void flush();
	void drawSprite(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color, int textureID);

private:
	void createVertexArray();
	void createVertexBuffer();
	void createIndexBuffer();

	std::unique_ptr<VAO> vao;
	std::unique_ptr<VBO> vbo;
	std::unique_ptr<EBO> ebo;
	std::shared_ptr<Shader> shader;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	int maxSprites = 1000; // Maximum number of sprites per batch
};

#endif // !BATCH_RENDERER_H

