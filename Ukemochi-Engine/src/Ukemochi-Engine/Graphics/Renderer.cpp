/* Start Header ************************************************************************/
/*!
\file       Renderer.cpp
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu (%)
\co-authors Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (%)
\co-authors Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu (%)
\date       Sept 25, 2024
\brief      This file contains the implementation of the Renderer class responsible for
			handling OpenGL rendering, including setting up shaders, buffers, textures,
			and rendering 2D objects like boxes and circles.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "../Factory/Factory.h"
#include "../Factory/GameObjectManager.h"
#include "Ukemochi-Engine/SceneManager.h"
#include "UIButton.h"

using namespace Ukemochi;

/*!
 * @brief Constructor for the Renderer class.
 * Initializes pointers to OpenGL objects (e.g., shaderProgram, VAOs, VBOs, EBOs) to nullptr.
 */
Renderer::Renderer()
{
	// Pointers to OpenGL objects are set to nullptr initially
	shaderProgram = nullptr;
	vaos.clear();
	vbos.clear();
	ebos.clear();

	framebuffer = 0;
	textureColorbuffer = 0;
	rbo = 0;
	screenQuadVAO = nullptr;
	screenQuadVBO = nullptr;
	framebufferShader = nullptr;
	textRenderer = nullptr;
	playerObject = nullptr;
};

/*!
 * @brief Destructor for the Renderer class.
 * Calls the cleanUp() method to release all allocated OpenGL resources.
 */
Renderer::~Renderer()
{
	cleanUp();
	glfwTerminate();

}

/*!
 * @brief Initializes the renderer, including buffers, and text renderer.
 */
void Renderer::init()
{

	// Load Buffers for debug/wireframe box drawing
	initDebugBoxBuffers();

	// Load Buffers for circle drawing
	initCircleBuffers();

	// Load Buffers for debug/wireframe circle drawing
	initCircleOutlineBuffers();


	setupFramebuffer();
	initScreenQuad();

	setUpObjectPickingBuffer();
	setupColorPickingFramebuffer();
	assignUniqueColorsToEntities();

	// Text Rendering (Test)
	// Initialize text renderer with screen dimensions
	textRenderer = new TextRenderer(screen_width, screen_height);

	// Load multiple fonts into the text renderer
	textRenderer->loadTextFont("Ukemochi", "../Assets/Fonts/Ukemochi_font-Regular.ttf");
	textRenderer->loadTextFont("Exo2", "../Assets/Fonts/Exo2-Regular.ttf");

	// Add text objects
	//textRenderer->addTextObject("title", TextObject("Ukemochi!", glm::vec2(50.0f, 800.f), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), "Ukemochi"));
	//textRenderer->addTextObject("subtitle", TextObject("Exo2!", glm::vec2(50.0f, 750.f), 1.0f, glm::vec3(0.5f, 0.8f, 0.2f), "Exo2"));

	// initAnimationEntities();
	
	//particleSystem = std::make_unique<ParticleSystem>(particleShader, );

	batchRenderer = std::make_unique<BatchRenderer2D>();
	// Load shaders and create shared pointer

	batchRenderer->init(shaderProgram);

	UIRenderer = std::make_unique<UIButtonRenderer>(batchRenderer, textRenderer, screen_width, screen_height, UI_shader_program);

	// Add buttons
	//UIRenderer->addButton(UIButton("pauseButton",
	//	glm::vec2(100.0f, 700.0f),
	//	glm::vec2(150.0f, 100.0f),
	//	5,  // Replace with actual texture ID
	//	"hi",
	//	glm::vec3(1.0f, 0.f, 0.f),
	//	"Exo2",
	//	1.0f
	//));
}

/*!
 * @brief Sets up the framebuffer for off-screen rendering.
 */
void Renderer::setupFramebuffer()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);


	// Create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	else
	{
		std::cout << "Framebuffer setup complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*!
 * @brief Resizes the framebuffer and adjusts the color and render buffers to fit the new dimensions.
 * @param width The new width for the framebuffer.
 * @param height The new height for the framebuffer.
 */
void Renderer::resizeFramebuffer(unsigned int width, unsigned int height) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	// Resize color texture
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(width), static_cast<int>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// Resize renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<int>(width), static_cast<int>(height));

	glBindFramebuffer(GL_FRAMEBUFFER,0);

	// May as well update viewport?
	glViewport(0,0,width,height);
}

/*!
 * @brief Initializes the screen quad for rendering the framebuffer texture to the screen.
 */
void Renderer::initScreenQuad()
{
	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	screenQuadVAO = std::make_unique<VAO>();
	screenQuadVBO = std::make_unique<VBO>(quadVertices, sizeof(quadVertices));

	screenQuadVAO->Bind();
	screenQuadVBO->Bind();
	screenQuadVAO->LinkAttrib(*screenQuadVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
	screenQuadVAO->LinkAttrib(*screenQuadVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Create a shader for rendering the framebuffer texture
	//framebufferShader = std::make_unique<Shader>("../Assets/Shaders/framebuffer.vert", "../Assets/Shaders/framebuffer.frag");

	ECS::GetInstance().GetSystem<AssetManager>()->addShader("framebuffer", "../Assets/Shaders/framebuffer.vert", "../Assets/Shaders/framebuffer.frag");
	framebufferShader = ECS::GetInstance().GetSystem<AssetManager>()->getShader("framebuffer");

}

/*!
 * @brief Begins rendering to the framebuffer.
 */
void Renderer::beginFramebufferRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*!
 * @brief Ends rendering to the framebuffer.
 */
void Renderer::endFramebufferRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderScreenQuad()
{
	framebufferShader->Activate();
	screenQuadVAO->Bind();
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

/*!
 * @brief Renders the framebuffer texture to the screen.
 */
void Renderer::renderToFramebuffer()
{
	//beginFramebufferRender();

	// Add debug information
	//std::cout << "Begin framebuffer render" << std::endl;
	//std::cout << "Number of entities: " << m_Entities.size() << std::endl;

	// Perform your regular rendering here
	render();

	//std::cout << "End framebuffer render" << std::endl;

	endFramebufferRender();

	// Now render the framebuffer texture to the screen
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//renderScreenQuad();
	//glEnable(GL_DEPTH_TEST);
}

/*!
 * @brief Gets the color buffer texture of the framebuffer.
 * @return The texture ID of the framebuffer's color buffer.
 */
GLuint Renderer::getTextureColorBuffer() const
{
	return textureColorbuffer;  // this is framebuffer's color texture
}

/*!
 * @brief Initializes vertex and index buffers for drawing a wireframe outline of a 2D box.
 */
void Renderer::initDebugBoxBuffers()
{
	// Define vertices for a box (centered around origin)
	GLfloat vertices_box[] = {
		-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,   // Top-left
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,   // Bottom-left
		 0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,   // Bottom-right
		 0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f    // Top-right
	};

	// Define indices for drawing the outline of the box using GL_LINES
	GLuint indices_box[] = {
		0, 1, // Top-left to Bottom-left
		2, 3 // Top-right to Top-left
	};

	// Set up the buffers once, and bind the VAO/VBO/EBO
	setUpBuffers(vertices_box, sizeof(vertices_box), indices_box, sizeof(indices_box));

	// Store the number of indices to be drawn, which is 8 (4 pairs of vertices)
	indices_count.push_back(4); // 4 lines with 2 vertices each
}

/*!
 * @brief Initializes vertex and index buffers for drawing a 2D circle with texture mapping.
 * @param segments The number of segments to approximate the circle.
 */
void Renderer::initCircleBuffers(GLuint segments)
{

	// Arrays for vertices and indices
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	// Add center vertex (0, 0 in NDC space)
	vertices.push_back(0.0f);  // X
	vertices.push_back(0.0f);  // Y
	vertices.push_back(0.0f);  // Z
	vertices.push_back(1.0f);  // Color (r)
	vertices.push_back(1.0f);  // Color (g)
	vertices.push_back(1.0f);  // Color (b)
	vertices.push_back(0.5f);  // Texture coordinate (s) - center of the texture
	vertices.push_back(0.5f);  // Texture coordinate (t)

	// Generate vertices around the circle
	for (GLuint i = 0; i <= segments; ++i)
	{
		GLfloat angle = i * 2.0f * 3.1415926f / segments;
		GLfloat dx = cosf(angle);
		GLfloat dy = sinf(angle);

		// Add vertex position (unit circle in NDC)
		vertices.push_back(dx);
		vertices.push_back(dy);
		vertices.push_back(0.0f);  // Z coordinate

		// Color (white)
		vertices.push_back(1.0f);  // Color r
		vertices.push_back(1.0f);  // Color g
		vertices.push_back(1.0f);  // Color b

		// Texture coordinates
		vertices.push_back(0.5f + cosf(angle) * 0.5f);  // s (normalized)
		vertices.push_back(0.5f + sinf(angle) * 0.5f);  // t (normalized)

		// Index the vertices for triangle fan
		indices.push_back(i + 1);
	}

	// Add index 0 (center vertex) at the start of the indices array
	indices.insert(indices.begin(), 0);
	indices.push_back(1); // Close the loop

	// Set up buffers once
	setUpBuffers(vertices.data(), static_cast<GLuint>(vertices.size() * sizeof(GLfloat)), indices.data(), static_cast<GLuint>(indices.size() * sizeof(GLuint)));
}

/*!
 * @brief Initializes vertex and index buffers for drawing a wireframe outline of a 2D circle.
 * @param segments The number of segments to approximate the circle.
 */

void Renderer::initCircleOutlineBuffers(GLuint segments)
{
	std::vector<GLfloat> vertices;

	// Define the center position (0, 0)
	GLfloat z = 0.0f;

	// Generate vertices for the circle's outline
	for (size_t i = 0; i <= segments; ++i) {
		GLfloat angle = i * 2.0f * 3.1415926f / segments;
		GLfloat dx = cosf(angle);
		GLfloat dy = sinf(angle);
		vertices.push_back(dx);
		vertices.push_back(dy);
		vertices.push_back(z);  // Z (depth)

		// Color (white)
		vertices.push_back(1.0f);  // Color r
		vertices.push_back(1.0f);  // Color g
		vertices.push_back(1.0f);  // Color b

		// Texture coordinates
		vertices.push_back(0.f);  // s (normalized)
		vertices.push_back(0.f);  // t (normalized)
	}

	// Set up a VAO/VBO for the circle outline
	setUpBuffers(vertices.data(), vertices.size() * sizeof(GLfloat), nullptr, 0); // No indices since it's a line loop
}

/*!
 * @brief Loads and sets up a texture based on the given file path.
 * Supports PNG (GL_RGBA) and JPG (GL_RGB) formats.
 * @param texturePath The file path to the texture to be loaded.
 * @param textureIndex Reference to the texture index, incremented for each new texture loaded.
 */
void Renderer::setUpTextures(const std::string& texturePath, int& textureIndex)
{
	if (!texturePath.empty())
	{
		auto it = textureCache.find(texturePath);
		if (it != textureCache.end())
		{
			// Texture is already loaded; just return
			std::cout << "Texture " << texturePath << " is already loaded with ID: " << it->second->ID << " at index " << textureIndex << std::endl;
			return;
		}

		// Determine format based on file extension
		std::string extension = texturePath.substr(texturePath.find_last_of(".") + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		GLenum format = (extension == "png") ? GL_RGBA : GL_RGB;
		Texture* texture = new Texture(texturePath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0 + textureIndex, format, GL_UNSIGNED_BYTE);

		// Set the texture to the specific index in the shader array
		std::string uniformName = "textures[" + std::to_string(textureIndex) + "]";
		texture->texUnit(shaderProgram.get(), uniformName.c_str(), textureIndex);

		textureCache[texturePath] = texture;
		texturePathsOrder.push_back(texturePath);

		textureIndex++;
	}

}

/*!
 * @brief Binds textures to OpenGL texture units and sets them up in the shader.
 * @param shader The shader program to bind textures to.
 */
void Renderer::bindTexturesToUnits(std::shared_ptr<Shader> shader)
{
	// Set textureCount based on the number of unique textures, limited to 32
	//int textureCount = std::min(32, static_cast<int>(texturePathsOrder.size()));
	int texture_order_count = static_cast<int>(ECS::GetInstance().GetSystem<AssetManager>()->texture_order.size());
	int textureCount = std::min(32, texture_order_count);
	std::vector<int> textureUnits(textureCount);


	for (int i = 0; i < /*texturePathsOrder.size()*/ texture_order_count && nextAvailableTextureUnit < 32; ++i) {
		/*const auto& path = texturePathsOrder[i];
		Texture* texture = textureCache[path];*/

		const auto& path = ECS::GetInstance().GetSystem<AssetManager>()->texture_order[i];
		Texture* texture = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(path).get();

		if (texture->ID == 0) {
			std::cerr << "Error: Failed to load texture for path: " << path << std::endl;
			continue;
		}

		// Check if this texture is already mapped to a unit
		if (textureIDMap.find(texture->ID) == textureIDMap.end()) {
			// New texture, assign to the next available texture unit
			textureIDMap[texture->ID] = nextAvailableTextureUnit;

			// Bind the texture to the OpenGL texture unit
			glActiveTexture(GL_TEXTURE0 + nextAvailableTextureUnit);
			glBindTexture(GL_TEXTURE_2D, texture->ID);

			textureUnits[i] = nextAvailableTextureUnit;

			// Increment to the next available texture unit
			nextAvailableTextureUnit++;
		}
		else {
			// Texture already mapped, retrieve existing texture unit
			textureUnits[i] = textureIDMap[texture->ID];
		}
	}
	/*for (int i = 0; i < textureCount; ++i) {
		const auto& path = texturePathsOrder[i];
		Texture* texture = textureCache[path];*/

	for (int i{}; i < ECS::GetInstance().GetSystem<AssetManager>()->order_index; i++)
	{
		const auto& path = ECS::GetInstance().GetSystem<AssetManager>()->texture_order[i];
		Texture* texture = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(path).get();

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texture->ID);

		// Verify that the texture is bound to the expected unit
		GLint boundTexture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);

	}
	// Pass the array of texture unit indices to the shader uniform array "textures"
	shader->setIntArray("textures", textureUnits.data(), textureCount);
}



/*!
 * @brief Sets up and compiles shaders used by the renderer.
 */
void Renderer::setUpShaders()
{
	shaderProgram = std::make_shared<Shader>("../Assets/Shaders/default.vert", "../Assets/Shaders/default.frag");

	debug_shader_program = std::make_unique<Shader>("../Assets/Shaders/debug.vert", "../Assets/Shaders/debug.frag");

	UI_shader_program = std::make_shared<Shader>("../Assets/Shaders/UI.vert", "../Assets/Shaders/UI.frag");

	object_picking_shader_program = std::make_shared<Shader>("../Assets/Shaders/object_picking.vert", "../Assets/Shaders/object_picking.frag");

	pointShader = std::make_unique<Shader>("../Assets/Shaders/point.vert", "../Assets/Shaders/point.frag");
}

/*!
 * @brief Sets up the Vertex Array Object (VAO), Vertex Buffer Object (VBO), and Element Buffer Object (EBO) for the given vertices and indices.
 * This function prepares the OpenGL buffers for rendering the object.
 * @param vertices Pointer to the vertex data.
 * @param vertSize The size of the vertex data in bytes.
 * @param indices Pointer to the index data.
 * @param indexSize The size of the index data in bytes.
 */
void Renderer::setUpBuffers(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indexSize)
{
	// Create new VAO, VBO, and EBO for each object
	VAO* vao = new VAO();
	VBO* vbo = new VBO(vertices, vertSize);
	EBO* ebo = new EBO(indices, indexSize);

	vao->Bind();
	vbo->Bind();
	ebo->Bind();

	vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	vao->Unbind();
	vbo->Unbind();
	ebo->Unbind();

	// Store the buffers in the list
	vaos.push_back(vao);
	vbos.push_back(vbo);
	ebos.push_back(ebo);

}

/*!
 * @brief Renders all the objects (boxes and circles) using the set up VAOs, VBOs, EBOs, and textures.
 */
void Renderer::render()
{
	renderForObjectPicking();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Get the current time
	GLfloat currentFrameTime = static_cast<GLfloat>(glfwGetTime());  // This will return time in seconds
	deltaTime = currentFrameTime - lastFrame;
	lastFrame = currentFrameTime;

	// Clear the screen
#ifdef _DEBUG
	beginFramebufferRender();
#endif // _DEBUG

#ifndef _DEBUG
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif // !_DEBUG

	// Get the camera's view and projection matrices
	const auto& camera = ECS::GetInstance().GetSystem<Camera>();
	glm::mat4 view = camera->getCameraViewMatrix();
	glm::mat4 projection = camera->getCameraProjectionMatrix();

	// Set the projection and view matrices in the shader
	shaderProgram->Activate();
	bindTexturesToUnits(shaderProgram);
	shaderProgram->setMat4("view", view);
	shaderProgram->setMat4("projection", projection);


	// Render entities
	batchRenderer->beginBatch();
	
	for (auto& entity : m_Entities)
	{
		if (!GameObjectManager::GetInstance().GetGO(entity)->GetActive())
			continue;
		
		auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
		auto& spriteRenderer = ECS::GetInstance().GetComponent<SpriteRender>(entity);

		// Set up the model matrix
		glm::mat4 model{};
		
		// Copy elements from custom matrix4x4 to glm::mat4
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				model[i][j] = transform.transform_matrix.m2[j][i];

		// Initialize UV coordinates
		GLfloat uvCoordinates[8];
		GLint textureID = -1;
		
		// Check for an Animator component
		if(ECS::GetInstance().HasComponent<Animation>(entity))
		{
			auto& ani = ECS::GetInstance().GetComponent<Animation>(entity);
			auto& clip = ani.clips[ani.currentClip];
			spriteRenderer.texturePath = clip.keyPath;
			updateAnimationFrame(ani.current_frame, clip.pixel_width, clip.pixel_height, clip.total_width, clip.total_height, uvCoordinates);
			
			glm::vec2 pos = glm::vec2(transform.position.x, transform.position.y);
			glm::vec2 offset = glm::vec2(static_cast<float>(clip.pixel_width) * (0.5f - clip.pivot.x), static_cast<float>(clip.pixel_height) * (0.5f - clip.pivot.y));
			glm::vec2 renderPos = pos;
			if(spriteRenderer.flipX)
			{
				std::swap(uvCoordinates[0], uvCoordinates[2]); // Bottom-left <-> Bottom-right
				std::swap(uvCoordinates[1], uvCoordinates[3]);
				std::swap(uvCoordinates[4], uvCoordinates[6]); // Top-right <-> Top-left
				std::swap(uvCoordinates[5], uvCoordinates[7]);

				renderPos.x += offset.x;
				renderPos.y -= offset.y;
			}
			else
				renderPos -= offset;

			if (ECS::GetInstance().GetSystem<AssetManager>()->texture_list.find(spriteRenderer.texturePath) != 
				ECS::GetInstance().GetSystem<AssetManager>()->texture_list.end())
			{
				textureID = ECS::GetInstance().GetSystem<AssetManager>()->texture_list[spriteRenderer.texturePath]->ID;
			}
			if (textureID < 0) {
				UME_ENGINE_WARN("Texture ID not found for {0}", spriteRenderer.texturePath);
				continue;
			}

			int mappedTextureUnit = textureIDMap[textureID];

			static constexpr int TARGET_SCALE_FACTOR = 5;
			
			float aspectRatio = static_cast<float>(clip.pixel_width) / static_cast<float>(clip.pixel_height);
			glm::vec2 spriteWorldSize = glm::vec2(static_cast<float>(clip.pixel_width), static_cast<float>(clip.pixel_height)) / glm::vec2(
				static_cast<float>(clip.pixelsPerUnit));
			float scaleFactor = TARGET_SCALE_FACTOR / spriteWorldSize.y;
			glm::vec2 finalScale = glm::vec2(transform.scale.x, transform.scale.y) * scaleFactor;
			batchRenderer->drawSprite(renderPos, finalScale, glm::vec3(1.0f, 1.0f, 1.0f), mappedTextureUnit, uvCoordinates, glm::radians(transform.rotation));
		}
		else
		{
			// Use default full texture UVs for static sprites
			uvCoordinates[0] = 0.0f; uvCoordinates[1] = 0.0f;  // Bottom-left
			uvCoordinates[2] = 1.0f; uvCoordinates[3] = 0.0f;  // Bottom-right
			uvCoordinates[4] = 1.0f; uvCoordinates[5] = 1.0f;  // Top-right
			uvCoordinates[6] = 0.0f; uvCoordinates[7] = 1.0f;  // Top-left
		
			// Flip UVs for static sprites
			if(spriteRenderer.flipX)
			{
				std::swap(uvCoordinates[0], uvCoordinates[2]); // Bottom-left <-> Bottom-right
				std::swap(uvCoordinates[1], uvCoordinates[3]);
				std::swap(uvCoordinates[4], uvCoordinates[6]); // Top-right <-> Top-left
				std::swap(uvCoordinates[5], uvCoordinates[7]);
			}

			if (ECS::GetInstance().GetSystem<AssetManager>()->texture_list.find(spriteRenderer.texturePath) != 
				ECS::GetInstance().GetSystem<AssetManager>()->texture_list.end())
			{
				textureID = ECS::GetInstance().GetSystem<AssetManager>()->texture_list[spriteRenderer.texturePath]->ID;
			}
			if (textureID < 0) {
				UME_ENGINE_WARN("Texture ID not found for {0}", spriteRenderer.texturePath);
				continue;
			}
			
			int mappedTextureUnit = textureIDMap[textureID];

			// Draw the sprite using the batch renderer, passing the updated UV coordinates
			batchRenderer->drawSprite(glm::vec2(transform.position.x, transform.position.y), glm::vec2(transform.scale.x, transform.scale.y), glm::vec3(1.0f, 1.0f, 1.0f), mappedTextureUnit, uvCoordinates, glm::radians(transform.rotation));
		}
	}


	batchRenderer->endBatch();

	UIRenderer->renderButtons(*camera);
	// Render debug wireframes if debug mode is enabled
	if (debug_mode_enabled)
	{
		debug_shader_program->Activate();
		debug_shader_program->setMat4("view", view);
		debug_shader_program->setMat4("projection", projection);

		for (auto& entity : m_Entities)
		{
			auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
			auto& spriteRenderer = ECS::GetInstance().GetComponent<SpriteRender>(entity);

			// Set up model matrix for the debug outline
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(transform.position.x, transform.position.y, 0.0f));
			model = glm::rotate(model, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(transform.scale.x, transform.scale.y, 1.0f));
			debug_shader_program->setMat4("model", model);

			// Draw box or circle outline depending on the entity shape
			if (spriteRenderer.shape == SPRITE_SHAPE::BOX) {
				drawBoxOutline();  // This function uses GL_LINE_LOOP to draw the outline
			}
			else if (spriteRenderer.shape == SPRITE_SHAPE::CIRCLE) {
				drawCircleOutline();  // Assuming you have a similar function for circles
			}
		}
		debug_shader_program->Deactivate();
	}

	// Render text, UI, or additional overlays if needed
	textRenderer->renderAllText();
}

void Renderer::handleMouseClick(int mouseX, int mouseY) 
{

	for (auto& entity : m_Entities) 
	{
		auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
		auto& spriteRenderer = ECS::GetInstance().GetComponent<SpriteRender>(entity);

		glm::vec2 entityPos(transform.position.x, transform.position.y);
		glm::vec2 entitySize(transform.scale.x, transform.scale.y);

		// Check if the mouse is within the entity's bounding box
		if ((mouseX >= entityPos.x - (entitySize.x / 2)) && (mouseX <= entityPos.x + (entitySize.x / 2)) &&
			(mouseY <= entityPos.y + (entitySize.y / 2)) && (mouseY >= entityPos.y - (entitySize.y / 2)) ) 
		{
			std::cout << "Clicked on entity ID: " << entity << std::endl;

			// Perform any additional logic here (e.g., highlight or select the entity)
			break;
		}
	}
}


/*!
 * @brief Cleans up and releases all OpenGL resources (VAOs, VBOs, EBOs, textures, shaders).
 */
void Renderer::cleanUp()
{
	// Delete all VAOs, VBOs, and EBOs
	for (size_t i = 0; i < vaos.size(); ++i)
	{
		vaos[i]->Delete();
		delete vaos[i]; // Deallocate memory
	}
	for (size_t i = 0; i < vbos.size(); ++i)
	{
		vbos[i]->Delete();
		delete vbos[i]; // Deallocate memory
	}
	for (size_t i = 0; i < ebos.size(); ++i)
	{
		ebos[i]->Delete();
		delete ebos[i]; // Deallocate memory
	}

	// Clear the vectors after deleting the objects
	vaos.clear();
	vbos.clear();
	ebos.clear();

	for (auto& texture : textureCache)
	{
		if (texture.second)
		{
			texture.second->Delete();
		}
		delete texture.second;
	}

	// Clear the textures vector
	textures.clear();

	// Clear the textures_enabled vector as well
	textures_enabled.clear();

	// Clear texture cache
	textureCache.clear();

	texturePathsOrder.clear();
	textureIDMap.clear();

	if (framebuffer)
		glDeleteFramebuffers(1, &framebuffer);
	framebuffer = 0;

	if (textureColorbuffer)
		glDeleteTextures(1, &textureColorbuffer);
	textureColorbuffer = 0;

	if (rbo)
		glDeleteRenderbuffers(1, &rbo);
	rbo = 0;
	if (textRenderer)
	{
		delete textRenderer;
		textRenderer = nullptr;
	}

	if (objectPickingFrameBuffer)
		glDeleteFramebuffers(1, &objectPickingFrameBuffer);
	objectPickingFrameBuffer = 0;

	if (colorPickingBuffer)
		glDeleteTextures(1, &colorPickingBuffer);
	colorPickingBuffer = 0;

	if (object_picking_rbo)
		glDeleteRenderbuffers(1, &object_picking_rbo);
}

/*!
 * @brief Draws a 2D circle with the given position, radius, and texture,
		  starting position is the top left of screen. It starts from the
		  center of the box.
 */
void Renderer::drawCircle()
{
	// Bind the VAO for the circle
	vaos[CIRCLE_VAO]->Bind();

	// Set the object color (if necessary)
	shaderProgram->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));  // White color

	// Draw the circle using GL_TRIANGLE_FAN
	glDrawElements(GL_TRIANGLE_FAN, 1002, GL_UNSIGNED_INT, 0);

	// Unbind the VAO after drawing
	vaos[CIRCLE_VAO]->Unbind();
}

/*!
 * @brief Toggles scale transformation for objects.
 */
void Renderer::ToggleInputsForScale()
{
	scale_enabled = static_cast<GLboolean>(!scale_enabled);
	// Adjust scale factor when toggled
	if (scale_enabled)
		scale_factor = 0.5f;
	else
		scale_factor = 1.0f;
}

/*!
 * @brief Toggles rotation transformation for objects.
 */
void Renderer::ToggleInputsForRotation()
{
	rotation_enabled = static_cast<GLboolean>(!rotation_enabled);
	if (!rotation_enabled)
	{
		rotation_angle = 0.f;
	}
}

/*!
 * @brief Draws the outline of a 2D box using a wireframe.
 */
void Renderer::drawBoxOutline()
{
	debug_shader_program->Activate();
	// Bind the VAO specifically for drawing outlines
	vaos[BOX_OUTLINE]->Bind();

	// Set the line width for better visibility (optional)
	glLineWidth(2.0f);

	// Set the wireframe color to red
	debug_shader_program->setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));  // Set uniform to red

	// Draw the outline using GL_LINE_LOOP to draw only the edges
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

	vaos[BOX_OUTLINE]->Unbind();
	debug_shader_program->Deactivate();
}

/*!
 * @brief Draws the outline of a 2D circle using a wireframe.
 */
void Renderer::drawCircleOutline()
{
	debug_shader_program->Activate();
	// Bind the VAO
	vaos[CIRCLE_OUTLINE]->Bind();

	// Set the wireframe color to red
	debug_shader_program->setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));  // Set uniform to red

	// Draw the outline of the circle using GL_LINE_LOOP
	glDrawArrays(GL_LINE_LOOP, 0, 1000);

	// Unbind the VAO
	vaos[CIRCLE_OUTLINE]->Unbind();
}

/*!
 * @brief Updates the UV coordinates for the current frame of an animation.
 * @param currentFrame The current frame index.
 * @param frameWidth Width of each frame in pixels.
 * @param frameHeight Height of each frame in pixels.
 * @param totalWidth Total width of the sprite sheet.
 * @param totalHeight Total height of the sprite sheet.
 * @param uvCoordinates Output UV coordinates for the current frame.
 */
void Renderer::updateAnimationFrame(int currentFrame, int frameWidth, int frameHeight, int totalWidth, int totalHeight, GLfloat* uvCoordinates)
{
	// Calculate column and row of the current frame
	int column = currentFrame % (totalWidth / frameWidth);
	int row = currentFrame / (totalWidth / frameWidth);

	// Calculate normalized UV coordinates
	float uvX = (column * frameWidth) / static_cast<float>(totalWidth);
	float uvY = 1.0f - ((row + 1) * frameHeight) / static_cast<float>(totalHeight); // Adjusted to flip vertically
	float uvWidth = frameWidth / static_cast<float>(totalWidth);
	float uvHeight = frameHeight / static_cast<float>(totalHeight);

	// Set UV coordinates with OpenGL bottom-left orientation
	uvCoordinates[0] = uvX;                 // Bottom-left
	uvCoordinates[1] = uvY;
	uvCoordinates[2] = uvX + uvWidth;       // Bottom-right
	uvCoordinates[3] = uvY;
	uvCoordinates[4] = uvX + uvWidth;       // Top-right
	uvCoordinates[5] = uvY + uvHeight;
	uvCoordinates[6] = uvX;                 // Top-left
	uvCoordinates[7] = uvY + uvHeight;
}

/*!
 * @brief Draws an animated 2D box.
 */
void Renderer::drawBoxAnimation()
{
	// Activate the shader program and bind the texture.
	shaderProgram->Activate();

	// Draw the box using the pre-initialized VAO.
	vaos[ANIMATION_VAO]->Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	vaos[ANIMATION_VAO]->Unbind();
}

/*!
 * @brief Creates a text object in the text renderer with the specified properties.
 * @param id Unique identifier for the text object.
 * @param label Text content of the object.
 * @param pos Position of the text in screen coordinates.
 * @param scale Scale of the text.
 * @param color Color of the text.
 * @param font_name Font to be used for rendering the text.
 */
void Renderer::CreateTextObject(const std::string& id, const std::string& label, const Ukemochi::Vec2& pos, const float scale, const Ukemochi::Vec3& color, const std::string& font_name)
{
	textRenderer->addTextObject(id, TextObject(label, glm::vec2(pos.x, pos.y), scale, glm::vec3(color.x, color.y, color.z), font_name));
}

/*!
 * @brief Updates the text of an existing text object.
 * @param id Identifier of the text object.
 * @param newText The new text content.
 */
void Renderer::UpdateTextObject(const std::string& id, const std::string& newText) { textRenderer->updateTextObject(id, newText); }

void Renderer::CreateButtonObject(const std::string& id, const Ukemochi::Vec2& position, const Ukemochi::Vec2& size, int textureID, const std::string& text, const Ukemochi::Vec3& textColor, std::string fontName, float textScale, TextAlignment alignment, bool interactable, std::function<void()> on_click)
{
	UIRenderer->addButton(UIButton(id, glm::vec2(position.x, position.y), glm::vec2(size.x, size.y), GLuint(textureID), text, glm::vec3(textColor.x, textColor.y, textColor.z), fontName, textScale, alignment, interactable, on_click));
}

std::vector<UIButton>& Renderer::GetButtonObjects() { return UIRenderer->GetButtons(); }

/*!
 * @brief Initializes animation entities, creating idle and running animations for the player entity.
 */
void Renderer::initAnimationEntities()
{
	// size_t playerEntityID = GetPlayer(); // Replace with actual entity IDs from your ECS or game logic
	//
	// // Create animations for the player
	// Animation idleAnimation(37, 442, 448, 4096, 4096, 0.05f, true); 
	// Animation runAnimation(13, 461, 428, 2048, 2048, 0.1f, true); 
	//
	// // Add multiple animations for the player entity (idle and running animations)
	// entity_animations[playerEntityID] = { idleAnimation, runAnimation };

}

/*!
 * @brief Toggles slow-motion mode by adjusting animation frame duration.
 */
void Renderer::toggleSlowMotion()
{
	// Toggle slow-motion state
	isSlowMotion = !isSlowMotion;

	// Loop through all entities and adjust the frame duration for their animations
	for (auto& entity : m_Entities)
	{
		auto& spriteRenderer = ECS::GetInstance().GetComponent<SpriteRender>(entity);
		if (spriteRenderer.animated)
		{
			// Get the animations associated with this entity
			// auto& animations = entity_animations[entity];
			// for (auto& animation : animations)
			// {
			// 	// // Adjust the frame duration based on slow-motion state
			// 	// if (isSlowMotion)
			// 	// {
			// 	// 	animation.setFrameDuration(animation.originalFrameDuration * slowMotionFactor);
			// 	// }
			// 	// else
			// 	// {
			// 	// 	animation.resetFrameDuration(); // Reset to original duration
			// 	// }
			// }
		}
	}
}

/*!
 * @brief Handles key inputs for switching between idle and running animations based on movement keys.
 */
void Renderer::animationKeyInput()
{
	std::vector<GameObject*> list = GameObjectManager::GetInstance().GetAllGOs();
	if (list.empty())
	{
		return;
	}
	for (auto& GameObject : list)
	{
		if (GetPlayer() == GameObject->GetInstanceID())
		{
			// auto& playerSprite = GameObject->GetComponent<SpriteRender>();

			// File paths for the textures
			std::string runningTexturePath = "../Assets/Textures/running_player_sprite_sheet.png";
			std::string idleTexturePath = "../Assets/Textures/idle_player_sprite_sheet.png";

			// if (Input::IsKeyPressed(GLFW_KEY_A)) {
			// 	isFacingRight = false; // Moving left
			// }
			// else if (Input::IsKeyPressed(GLFW_KEY_D)) {
			// 	isFacingRight = true; // Moving right
			// }

			// Check if any movement keys are pressed
			// if (Input::IsKeyPressed(GLFW_KEY_W) ||
			// 	Input::IsKeyPressed(GLFW_KEY_A) ||
			// 	Input::IsKeyPressed(GLFW_KEY_S) ||
			// 	Input::IsKeyPressed(GLFW_KEY_D))
			// {
			// 	// If we are not already in the running state, switch to the running texture
			// 	if (playerSprite.animationIndex != 1)
			// 	{
			// 		playerSprite.animationIndex = 1;
			// 		playerSprite.texturePath = runningTexturePath;
			//
			// 		// Set the animation index and texture path to indicate running state
			// 		std::cout << "Switching to running animation.\n";
			// 	}
			// }
			// else
			// {
			// 	// If no movement keys are pressed and we are not in the idle state, switch to the idle texture
			// 	if (playerSprite.animationIndex != 0)
			// 	{
			// 		playerSprite.animationIndex = 0;
			// 		playerSprite.texturePath = idleTexturePath;
			//
			// 		// Set the animation index and texture path to indicate idle state
			// 		std::cout << "Switching to idle animation.\n";
			// 	}
			// }
			break;
		}
	}
}

void Renderer::assignUniqueColorsToEntities()
{
	size_t entityID = 0; // Starting ID
    for (auto& entity : m_Entities)
    {
        float r = ((entityID >> 16) & 0xFF) / 255.0f;  // Extract red
        float g = ((entityID >> 8) & 0xFF) / 255.0f;   // Extract green
        float b = (entityID & 0xFF) / 255.0f;          // Extract blue
        entityColors[entity] = glm::vec3(r, g, b);
        entityID++;
    }
}

glm::vec3 Renderer::encodeIDToColor(int id)
{
	float r = ((id >> 16) & 0xFF) / 255.0f;
	float g = ((id >> 8) & 0xFF) / 255.0f;
	float b = (id & 0xFF) / 255.0f;

	// Clamp the values to ensure they stay in the range [0.0, 1.0]
	return glm::vec3(glm::clamp(r, 0.0f, 1.0f),
		glm::clamp(g, 0.0f, 1.0f),
		glm::clamp(b, 0.0f, 1.0f));
}

void Renderer::setupColorPickingFramebuffer()
{
	glGenFramebuffers(1, &objectPickingFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, objectPickingFrameBuffer);

	glGenTextures(1, &colorPickingBuffer);
	glBindTexture(GL_TEXTURE_2D, colorPickingBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorPickingBuffer, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "ERROR: Framebuffer is not complete for color picking." << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderForObjectPicking()
{

	glBindFramebuffer(GL_FRAMEBUFFER, objectPickingFrameBuffer);
	glBindTexture(GL_TEXTURE_2D, colorPickingBuffer);
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glDisable(GL_BLEND); 
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get the camera's view and projection matrices
	const auto& camera = ECS::GetInstance().GetSystem<Camera>();
	glm::mat4 view = camera->getCameraViewMatrix();
	glm::mat4 projection = camera->getCameraProjectionMatrix();

	// Set the projection and view matrices in the shader
	object_picking_shader_program->Activate();
	object_picking_shader_program->setMat4("view", view);
	object_picking_shader_program->setMat4("projection", projection);

	for (auto& entity : m_Entities)
	{
		//std::cout << entity << std::endl;

		//glm::vec3 color = entityColors[entity];
		glm::vec3 color = encodeIDToColor(static_cast<int>(entity));
		//std::cout << "Encoded Color: (" << (int)(color.r * 255) << ", " << (int)(color.g * 255) << ", " << (int)(color.b * 255) << ")" << std::endl;

		object_picking_shader_program->setVec3("objectColor", encodeIDToColor(static_cast<int>(entity)));

		auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
		auto& spriteRenderer = ECS::GetInstance().GetComponent<SpriteRender>(entity);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(transform.position.x, transform.position.y, 0.0f));
		model = glm::rotate(model, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(transform.scale.x, transform.scale.y, 1.0f));
		object_picking_shader_program->setMat4("model", model);

		drawBox();


	}
	// Get mouse world position
	Vec2 mouse = SceneManager::GetInstance().GetPlayScreen();

	// Visualize the mouse position as a red point
	//drawPoint(mouse.x, mouse.y, glm::vec3(1.0f, 0.0f, 0.0f));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Now render the framebuffer texture to the screen
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	object_picking_shader_program->Deactivate();
}

size_t Renderer::getEntityFromMouseClick(int mouseX, int mouseY)
{
	glBindFramebuffer(GL_FRAMEBUFFER, objectPickingFrameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	unsigned char pixel[3];
	glReadPixels(mouseX, mouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Check if the color is the clear color
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255) {
		std::cout << "No entity found at (" << mouseX << ", " << mouseY << ")" << std::endl;
		return -1; // Sentinel for no entity
	}

	size_t entityID = (pixel[0] << 16) | (pixel[1] << 8) | pixel[2];

	// Debug output
	std::cout << "Mouse Click at (" << mouseX << ", " << mouseY
		<< ") " << std::endl;
	std::cout << "Pixel RGB: [" << (int)pixel[0] << ", "
		<< (int)pixel[1] << ", " << (int)pixel[2] << "]" << std::endl;
	std::cout << "Decoded Entity ID: " << entityID << std::endl;
	return entityID;
}

void Renderer::setUpObjectPickingBuffer()
{
	float quadVertices[] = {
	-0.5f,  0.5f, 0.0f, // Top-left
	-0.5f, -0.5f, 0.0f, // Bottom-left
	 0.5f, -0.5f, 0.0f, // Bottom-right
	 0.5f,  0.5f, 0.0f  // Top-right
	};

	GLuint indices[] = {
		0, 1, 2, // First triangle
		0, 2, 3  // Second triangle
	};

	objectPickingVAO = std::make_unique<VAO>();
	objectPickingVBO = std::make_unique<VBO>(quadVertices, sizeof(quadVertices));
	objectPickingEBO = std::make_unique<EBO>(indices, sizeof(indices));

	objectPickingVAO->Bind();
	objectPickingVBO->Bind();
	objectPickingEBO->Bind(); // Bind the EBO here
	objectPickingVAO->LinkAttrib(*objectPickingVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	// Create a shader for rendering the framebuffer texture
	//framebufferShader = std::make_unique<Shader>("../Assets/Shaders/framebuffer.vert", "../Assets/Shaders/framebuffer.frag");

	// Unbind to clean up
	objectPickingVAO->Unbind();
	objectPickingVBO->Unbind();
	objectPickingEBO->Unbind();

	ECS::GetInstance().GetSystem<AssetManager>()->addShader("objectPickingFramebuffer", "../Assets/Shaders/object_picking.vert", "../Assets/Shaders/object_picking.frag");
	object_picking_shader_program = ECS::GetInstance().GetSystem<AssetManager>()->getShader("objectPickingFramebuffer");
}

/*!
 * @brief Draws a 2D box with the given position, dimensions, and texture,
		  starting position is the top left of screen. It starts from the
		  center of the box.
 * @param x The x-coordinate of the center of the box (in screen space).
 * @param y The y-coordinate of the center of the box (in screen space).
 * @param width The width of the box (in screen space).
 * @param height The height of the box (in screen space).
 * @param texturePath The file path to the texture for the box.
 */
void Renderer::drawBox()
{
	//object_picking_shader_program->Activate();

	objectPickingVAO->Bind();

	glBindTexture(GL_TEXTURE_2D, colorPickingBuffer);
	// Issue the draw call using the pre-setup buffers
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	// Unbind the VAO[0] after drawing
	objectPickingVAO->Unbind();
}

GLuint Renderer::getObjectPickingColorBuffer() const
{
	return colorPickingBuffer;  // this is framebuffer's color texture
}

void Renderer::resizeObjectPickingFramebuffer(unsigned int width, unsigned int height) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, objectPickingFrameBuffer);

	// Resize color texture
	glBindTexture(GL_TEXTURE_2D, colorPickingBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<int>(width), static_cast<int>(height), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Resize renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, object_picking_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<int>(width), static_cast<int>(height));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// May as well update viewport?
	glViewport(0, 0, width, height);
}

void Renderer::drawPoint(float x, float y, glm::vec3 color)
{
	// Activate the point shader
	pointShader->Activate();

	// Set the color of the point
	pointShader->setVec3("pointColor", color);

	// Get the current camera projection and view matrices
	const auto& camera = ECS::GetInstance().GetSystem<Camera>();
	glm::mat4 projection = camera->getCameraProjectionMatrix();
	glm::mat4 view = camera->getCameraViewMatrix();

	// Pass matrices to the shader
	pointShader->setMat4("projection", projection);
	pointShader->setMat4("view", view);

	// Create a VAO and VBO for the point
	GLuint pointVAO, pointVBO;
	glGenVertexArrays(1, &pointVAO);
	glGenBuffers(1, &pointVBO);

	// Define the point position
	float pointVertices[] = { x, y };

	// Bind and configure the VAO/VBO
	glBindVertexArray(pointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertices), pointVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Draw the point
	glPointSize(10.0f); // Adjust the size of the point
	glDrawArrays(GL_POINTS, 0, 1);

	// Clean up
	glDeleteVertexArrays(1, &pointVAO);
	glDeleteBuffers(1, &pointVBO);
}

void Renderer::handleMouseClickOP(int mouseX, int mouseY)
{
	size_t entityID = getEntityFromMouseClick(mouseX, mouseY);
	if (entityID != -1 && ECS::GetInstance().HasComponent<Transform>(entityID))
	{
		selectedEntityID = entityID;
		isDragging = true;

		// Calculate the drag offset
		auto& transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);
		dragOffset.x = transform.position.x - mouseX;
		dragOffset.y = transform.position.y - mouseY;

		std::cout << "Selected entity ID: " << selectedEntityID << std::endl;
	}
	else
	{
		selectedEntityID = -1; // No valid entity selected
		isDragging = false;
		std::cout << "No valid entity at mouse click position." << std::endl;
	}
}

void Renderer::handleMouseDrag(int mouseX, int mouseY)
{
	if (isDragging && selectedEntityID != -1)
	{
		// Ensure the entity exists and has the required Transform component
		if (ECS::GetInstance().HasComponent<Transform>(selectedEntityID))
		{
			auto& transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);

			// Update position based on mouse and drag offset
			transform.position.x = mouseX + dragOffset.x;
			transform.position.y = mouseY + dragOffset.y;

			// Optional: Debug output to monitor dragging behavior
			std::cout << "Dragging entity " << selectedEntityID
				<< " to position (" << transform.position.x
				<< ", " << transform.position.y << ")" << std::endl;
		}
		else
		{
			std::cerr << "Error: Entity " << selectedEntityID
				<< " does not have a Transform component." << std::endl;
			isDragging = false;
		}
	}
}

