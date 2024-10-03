#include "Sprite.h"
#include "renderer.h"

namespace Ukemochi
{
	//Sprite::Sprite(float x, float y, float width, float height)
	//{
	//	// Convert screen coordinates to normalized device coordinates (NDC)
	//	float new_x = (2.0f * x) / Renderer::screen_width - 1.0f;
	//	float new_y = 1.0f - (2.0f * y) / Renderer::screen_height;
	//	float new_width = (2.0f * width) / Renderer::screen_width;
	//	float new_height = (2.0f * height) / Renderer::screen_height;

	//	float half_width = new_width / 2.0f;
	//	float half_height = new_height / 2.0f;

	//	// Define the vertices of the sprite
	//	vertices = std::unique_ptr<float>(new float[32] {
	//		-half_width, half_height, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
	//			-half_width, -half_height, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom left
	//			half_width, -half_height, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
	//			half_width, half_height, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f  // top right
	//		});

	//	// Define the indices of the sprite
	//	indices = std::unique_ptr<unsigned int>(new unsigned int[6] {
	//		0, 1, 3, // first triangle
	//			1, 2, 3  // second triangle
	//		});

	//	// Set up the buffers
	//	vao = std::make_unique<VAO>();
	//	vbo = std::make_unique<VBO>(vertices.get(), sizeof(vertices));
	//	ebo = std::make_unique<EBO>(indices.get(), sizeof(indices));

	//	vao->Bind();
	//	vbo->Bind();
	//	ebo->Bind();

	//	// Set the vertex attribute pointers
	//	vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	//	vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//	vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	//	vao->Unbind();
	//	vbo->Unbind();
	//	ebo->Unbind();
	//}
}