#pragma once

#include <glad/glad.h>
#include "PreCompile.h"
#include "../Math/Vector2D.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

namespace Ukemochi
{
	class Sprite {
		std::unique_ptr<VAO> vao; // Vertex Array Object
		std::unique_ptr<VBO> vbo; // Vertex Buffer Object
		std::unique_ptr<EBO> ebo; // Element Buffer Object

		std::unique_ptr<float> vertices;
		std::unique_ptr<unsigned int> indices;
	public:
		Sprite() = default;
		//Sprite(float x, float y, float width, float height);
		~Sprite() = default;

		void bindVAO() const { vao->Bind(); }
		void unbindVAO() const { vao->Unbind(); }
	};
}