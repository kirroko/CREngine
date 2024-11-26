#ifndef DEBUG_BATCH_RENDERING_H
#define DEBUG_BATCH_RENDERING_H

#include "PreCompile.h" 
#include "../vendor/glm/glm/glm.hpp"
#include "GLAD/glad.h"

// Forward declaration
class VAO;
class VBO;
class Shader;
class Texture;

struct debugVertex {
	glm::vec3 position;
};


class DebugBatchRenderer2D {

public:
	DebugBatchRenderer2D();
	~DebugBatchRenderer2D();

	void init(std::shared_ptr<Shader> debugShader);
	void beginBatch();
	void drawDebugBox(const glm::vec2& position, const glm::vec2& size, float rotation);
	void endBatch();
	void flush();

private:
	std::vector<debugVertex> vertices;
	std::unique_ptr<VAO> debug_vao;
	std::unique_ptr<VBO> debug_vbo;

	std::shared_ptr<Shader> shader;
	const int maxShapes = 1000;

};
#endif // !#DEBUG_BATCH_RENDERING_H
