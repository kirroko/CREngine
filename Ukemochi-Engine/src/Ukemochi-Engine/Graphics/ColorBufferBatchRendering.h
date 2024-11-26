#ifndef COLOR_BUFFER_BATCH_CLASS_H
#define COLOR_BUFFER_BATCH_CLASS_H

#include "PreCompile.h" 
#include "../vendor/glm/glm/glm.hpp"
#include "GLAD/glad.h"

// Forward declaration
class VAO;
class VBO;
class EBO;
class Shader;

struct ColorBufferVertex {
	glm::vec3 position;
	glm::vec3 color;
};

class ColorBufferBatchRenderer2D {
public:
	ColorBufferBatchRenderer2D();
	~ColorBufferBatchRenderer2D();

	void init(std::shared_ptr<Shader> colorBufferShader);
	void beginBatch();
	void drawDebugBox(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, float rotation);
	void endBatch();
	void flush();

private:
	std::vector<ColorBufferVertex> vertices;
	std::vector<GLuint> indices;
	std::unique_ptr<VAO> color_buffer_vao;
	std::unique_ptr<VBO> color_buffer_vbo;
	std::unique_ptr<EBO> color_buffer_ebo;

	std::shared_ptr<Shader> shader;
	const int maxBufferSprites = 1000;

};
#endif