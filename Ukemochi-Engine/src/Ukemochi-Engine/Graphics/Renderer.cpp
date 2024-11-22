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

	// Text Rendering (Test)
	// Initialize text renderer with screen dimensions
	textRenderer = new TextRenderer(screen_width, screen_height);

	// Load multiple fonts into the text renderer
	textRenderer->loadTextFont("Ukemochi", "../Assets/Fonts/Ukemochi_font-Regular.ttf");
	textRenderer->loadTextFont("Exo2", "../Assets/Fonts/Exo2-Regular.ttf");

	// Add text objects
	textRenderer->addTextObject("title", TextObject("Ukemochi!", glm::vec2(50.0f, 800.f), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), "Ukemochi"));
	textRenderer->addTextObject("subtitle", TextObject("Exo2!", glm::vec2(50.0f, 750.f), 1.0f, glm::vec3(0.5f, 0.8f, 0.2f), "Exo2"));

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
	beginFramebufferRender();

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
void Renderer::bindTexturesToUnits(std::shared_ptr<Shader> shader) //************** Look at it **************
{
	// Set textureCount based on the number of unique textures, limited to 32
	//int textureCount = std::min(32, static_cast<int>(texturePathsOrder.size()));
	int texture_order_count = static_cast<int>(ECS::GetInstance().GetSystem<AssetManager>()->getTextureOrder());
	int textureCount = std::min(32, texture_order_count);
	std::vector<int> textureUnits(textureCount);


	for (int i = 0; i < /*texturePathsOrder.size()*/ texture_order_count && nextAvailableTextureUnit < 32; ++i) {
		/*const auto& path = texturePathsOrder[i];
		Texture* texture = textureCache[path];*/

		const auto& path = ECS::GetInstance().GetSystem<AssetManager>()->getOrderAtIndex(i);
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

	for (int i{}; i < ECS::GetInstance().GetSystem<AssetManager>()->getTextureIndex(); i++)
	{
		const auto& path = ECS::GetInstance().GetSystem<AssetManager>()->getOrderAtIndex(i);
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
 * Gerald Edit: Tried to use AssetManager to access shader
 */
void Renderer::setUpShaders()
{
	//shaderProgram = std::make_shared<Shader>("../Assets/Shaders/default.vert", "../Assets/Shaders/default.frag");
	shaderProgram = ECS::GetInstance().GetSystem<AssetManager>()->getShader("default");

	debug_shader_program = std::make_unique<Shader>("../Assets/Shaders/debug.vert", "../Assets/Shaders/debug.frag");

	//UI_shader_program = std::make_shared<Shader>("../Assets/Shaders/UI.vert", "../Assets/Shaders/UI.frag");
	UI_shader_program = ECS::GetInstance().GetSystem<AssetManager>()->getShader("UI");
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
	debug_shader_program->Deactivate();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Get the current time
	GLfloat currentFrameTime = static_cast<GLfloat>(glfwGetTime());  // This will return time in seconds
	deltaTime = currentFrameTime - lastFrame;
	lastFrame = currentFrameTime;

	// Clear the screen
	glClearColor(0.07f, 0.13f, 0.17f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
		auto& spriteRenderer = ECS::GetInstance().GetComponent<SpriteRender>(entity);
		//auto& GameObject
		GameObject* go = GameObjectManager::GetInstance().GetGO(entity);
		if (go->GetTag() == "Button")
			std::cout << "Button" << std::endl;

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
		if (ECS::GetInstance().HasComponent<Animation>(entity))
		{
			auto& ani = ECS::GetInstance().GetComponent<Animation>(entity);
			if (ani.clips.find(ani.currentClip) == ani.clips.end())
			{
				UME_ENGINE_WARN("Clip not found for {0}, using default UV for now.", ani.currentClip);
				// Use default full texture UVs for static sprites
				uvCoordinates[0] = 0.0f; uvCoordinates[1] = 0.0f;  // Bottom-left
				uvCoordinates[2] = 1.0f; uvCoordinates[3] = 0.0f;  // Bottom-right
				uvCoordinates[4] = 1.0f; uvCoordinates[5] = 1.0f;  // Top-right
				uvCoordinates[6] = 0.0f; uvCoordinates[7] = 1.0f;  // Top-left
			}
			else
			{
				auto& clip = ani.clips[ani.currentClip];
				spriteRenderer.texturePath = clip.keyPath;
				updateAnimationFrame(ani.current_frame, clip.pixel_width, clip.pixel_height, clip.total_width, clip.total_height, uvCoordinates);
			}

			if (spriteRenderer.flipX)
			{
				std::swap(uvCoordinates[0], uvCoordinates[2]); // Bottom-left <-> Bottom-right
				std::swap(uvCoordinates[1], uvCoordinates[3]);
				std::swap(uvCoordinates[4], uvCoordinates[6]); // Top-right <-> Top-left
				std::swap(uvCoordinates[5], uvCoordinates[7]);
			}

			if (ECS::GetInstance().GetSystem<AssetManager>()->ifTextureExists(spriteRenderer.texturePath))
			{
				textureID = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(spriteRenderer.texturePath)->ID;
			}
			if (textureID < 0) {
				UME_ENGINE_WARN("Texture ID not found for {0}", spriteRenderer.texturePath);
				continue;
			}
		}
		else
		{
			// Use default full texture UVs for static sprites
			uvCoordinates[0] = 0.0f; uvCoordinates[1] = 0.0f;  // Bottom-left
			uvCoordinates[2] = 1.0f; uvCoordinates[3] = 0.0f;  // Bottom-right
			uvCoordinates[4] = 1.0f; uvCoordinates[5] = 1.0f;  // Top-right
			uvCoordinates[6] = 0.0f; uvCoordinates[7] = 1.0f;  // Top-left

			// Flip UVs for static sprites
			if (spriteRenderer.flipX)
			{
				std::swap(uvCoordinates[0], uvCoordinates[2]); // Bottom-left <-> Bottom-right
				std::swap(uvCoordinates[1], uvCoordinates[3]);
				std::swap(uvCoordinates[4], uvCoordinates[6]); // Top-right <-> Top-left
				std::swap(uvCoordinates[5], uvCoordinates[7]);
			}


			GLint textureID = -1;
			if (/*textureCache.find(spriteRenderer.texturePath) != textureCache.end()*/
				ECS::GetInstance().GetSystem<AssetManager>()->ifTextureExists(spriteRenderer.texturePath)) {
				textureID = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(spriteRenderer.texturePath)->ID;
			}
			if (textureID < 0) {
				std::cerr << "Warning: Texture ID not found for " << spriteRenderer.texturePath << std::endl;
				continue;
			}

			int mappedTextureUnit = textureIDMap[textureID];

			// Draw the sprite using the batch renderer, passing the updated UV coordinates
			batchRenderer->drawSprite(glm::vec2(transform.position.x, transform.position.y), glm::vec2(transform.scale.x, transform.scale.y), glm::vec3(1.0f, 1.0f, 1.0f), mappedTextureUnit, uvCoordinates, glm::radians(transform.rotation));
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
	// Bind the VAO[0] for the box
	vaos[0]->Bind();

	// Set the object color (if necessary)
	shaderProgram->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));  // White color

	// Issue the draw call using the pre-setup buffers
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Unbind the VAO[0] after drawing
	vaos[0]->Unbind();
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
