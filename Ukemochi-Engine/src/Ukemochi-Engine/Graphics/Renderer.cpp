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
#include "imgui.h"
#include "ImGuizmo.h"

using namespace Ukemochi;

std::string getSpriteNameFromPath(const std::string& texturePath)
{
	std::string fileName = std::filesystem::path(texturePath).filename().string();

	// Remove the extension if it exists
	size_t dotIndex = fileName.find_last_of('.');
	if (dotIndex != std::string::npos) {
		fileName = fileName.substr(0, dotIndex);
	}

	return fileName;
}
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

	// Text Rendering (Test)
	// Initialize text renderer with screen dimensions
	textRenderer = new TextRenderer(screen_width, screen_height);

	// Load multiple fonts into the text renderer
	textRenderer->loadTextFont("Ukemochi", "../Assets/Fonts/Ukemochi_font-Regular.ttf");
	textRenderer->loadTextFont("Ukemochi_numbers", "../Assets/Fonts/Ukemochi_numbers-Regular.ttf");

	// Add text objects
	//textRenderer->addTextObject("title", TextObject("Ukemochi!", glm::vec2(50.0f, 800.f), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), "Ukemochi"));
	//textRenderer->addTextObject("subtitle", TextObject("Exo2!", glm::vec2(50.0f, 750.f), 1.0f, glm::vec3(0.5f, 0.8f, 0.2f), "Exo2"));

	// initAnimationEntities();
	
	//particleSystem = std::make_unique<ParticleSystem>(particleShader, );

	batchRenderer = std::make_unique<BatchRenderer2D>();
	// Load shaders and create shared pointer

	batchRenderer->init(shaderProgram);

	UIRenderer = std::make_unique<UIButtonRenderer>(batchRenderer, textRenderer, screen_width, screen_height, UI_shader_program);

	debugBatchRenderer = std::make_unique<DebugBatchRenderer2D>();
	debugBatchRenderer->init(debug_shader_program);

	colorBufferBatchRenderer = std::make_unique<ColorBufferBatchRenderer2D>();
	colorBufferBatchRenderer->init(object_picking_shader_program);

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

	// Perform your regular rendering here
	render();


	endFramebufferRender();

	// Now render the framebuffer texture to the screen
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
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

	// Define vertices for a box below the origin (centered horizontally around origin)
	//GLfloat vertices_box[] = {
	//-0.5f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,   // Top-left
	//-0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,   // Bottom-left
	// 0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,   // Bottom-right
	// 0.5f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f    // Top-right
	//};


	//// Define indices for drawing the outline of the box using GL_LINES
	//GLuint indices_box[] = {
	//	0, 1, // Top-left to Bottom-left
	//	2, 3 // Top-right to Top-left
	//};

	//// Set up the buffers once, and bind the VAO/VBO/EBO
	//setUpBuffers(vertices_box, sizeof(vertices_box), indices_box, sizeof(indices_box));

	//// Store the number of indices to be drawn, which is 8 (4 pairs of vertices)
	//indices_count.push_back(4); // 4 lines with 2 vertices each
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
//void Renderer::bindTexturesToUnits(std::shared_ptr<Shader> shader)
//{
//	int texture_order_count = static_cast<int>(ECS::GetInstance().GetSystem<AssetManager>()->getTextureOrderSize());
//	int textureCount = std::min(32, texture_order_count);
//	std::vector<int> textureUnits(textureCount);
//
//	for (int i = 0; i < texture_order_count && i < 32; ++i) 
//	{
//		const auto& path = ECS::GetInstance().GetSystem<AssetManager>()->getTextureAtIndex(i);
//
//		// Skip atlas subtextures
//		if (ECS::GetInstance().GetSystem<AssetManager>()->isTextureInAtlas(path)) 
//		{
//			//std::cout << "Skipping texture (handled by atlas): " << path << std::endl;
//			continue;
//		}
//
//		Texture* texture = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(path).get();
//		if (!texture || texture->ID == 0) 
//		{
//			std::cerr << "Error: Failed to load texture for path: " << path << std::endl;
//			continue;
//		}
//
//		// Ensure no redundant binding
//		if (textureIDMap.find(texture->ID) == textureIDMap.end()) 
//		{
//			textureIDMap[texture->ID] = i;
//			textureUnits[i] = i;
//
//			glActiveTexture(GL_TEXTURE0 + i);
//			glBindTexture(GL_TEXTURE_2D, texture->ID);
//
//			/*std::cout << "Binding texture:\n"
//				<< "  Path: " << path << "\n"
//				<< "  Texture ID: " << texture->ID << "\n"
//				<< "  Assigned Unit: " << i << std::endl;*/
//		}
//		else 
//		{
//			textureUnits[i] = textureIDMap[texture->ID];
//		}
//	}
//
//	shader->setIntArray("textures", textureUnits.data(), textureCount);
//}

void Renderer::bindTexturesToUnits(std::shared_ptr<Shader> shader)
{
	// Set textureCount based on the number of unique textures, limited to 32 
	//int textureCount = std::min(32, static_cast<int>(texturePathsOrder.size())); 
	int texture_order_count = static_cast<int>(ECS::GetInstance().GetSystem<AssetManager>()->getTextureOrderSize());
	int textureCount = std::min(32, texture_order_count);
	std::vector<int> textureUnits(textureCount);


	for (int i = 0; i < /*texturePathsOrder.size()*/ texture_order_count && nextAvailableTextureUnit < 32; ++i) {
		/*const auto& path = texturePathsOrder[i];
		Texture* texture = textureCache[path];*/

		const auto& path = ECS::GetInstance().GetSystem<AssetManager>()->getTextureAtIndex(i);
		if (ECS::GetInstance().GetSystem<AssetManager>()->isTextureInAtlas(path))
		{
			//std::cout << "Skipping texture (handled by atlas): " << path << std::endl;
			continue;
		}
		Texture* texture = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(path).get();


		if (texture->ID == 0) {
			std::cerr << "Error: Failed to load texture for path: " << path << std::endl;
			continue;
		}

		// Check if this texture is already mapped to a unit 
		if (textureIDMap.find(texture->ID) == textureIDMap.end()) {
			// New texture, assign to the next available texture unit 
			textureIDMap[texture->ID] = i;

			// Bind the texture to the OpenGL texture unit 
			/*glActiveTexture(GL_TEXTURE0 + nextAvailableTextureUnit);
			glBindTexture(GL_TEXTURE_2D, texture->ID);*/

			textureUnits[i] = i;

			// Increment to the next available texture unit 
			//nextAvailableTextureUnit++; 
		}
		else {
			// Texture already mapped, retrieve existing texture unit 
			textureUnits[i] = textureIDMap[texture->ID];
		}
	}
	/*for (int i = 0; i < textureCount; ++i) {
	 const auto& path = texturePathsOrder[i];
	 Texture* texture = textureCache[path];*/

	for (int i{}; i < ECS::GetInstance().GetSystem<AssetManager>()->getTextureListSize(); i++)
	{
		const auto& path = ECS::GetInstance().GetSystem<AssetManager>()->getTextureAtIndex(i);
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

/* 
	FUNCTION I WANT TO USE BUT CANT BECAUSE OF GHOST TEXTURE 
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
//void Renderer::bindTexturesToUnits(std::shared_ptr<Shader> shader)
//{
//	int texture_order_count = static_cast<int>(ECS::GetInstance().GetSystem<AssetManager>()->getTextureOrderSize());
//	int textureCount = std::min(32, texture_order_count);
//	std::vector<int> textureUnits(textureCount);
//
//	// Keep track of bound atlas textures
//	std::unordered_set<std::string> boundAtlases;
//
//	for (int i = 0; i < texture_order_count && i < 32; ++i)
//	{
//		const auto& path = ECS::GetInstance().GetSystem<AssetManager>()->getTextureAtIndex(i);
//
//		// Determine if the texture is part of an atlas
//		bool isInAtlas = ECS::GetInstance().GetSystem<AssetManager>()->isTextureInAtlas(path);
//
//		if (isInAtlas)
//		{
//			// Retrieve the atlas texture path from the sprite data
//			const auto& spriteData = ECS::GetInstance().GetSystem<AssetManager>()->getSpriteData(path);
//			const std::string& atlasPath = spriteData.spriteSheetName;
//
//			// Skip if the atlas is already bound
//			if (boundAtlases.find(atlasPath) != boundAtlases.end())
//				continue;
//
//			Texture* atlasTexture = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(atlasPath).get();
//			if (!atlasTexture || atlasTexture->ID == 0)
//			{
//				std::cerr << "Error: Failed to load atlas texture: " << atlasPath << std::endl;
//				continue;
//			}
//
//			// Bind atlas texture if not already bound
//			if (textureIDMap.find(atlasTexture->ID) == textureIDMap.end())
//			{
//				textureIDMap[atlasTexture->ID] = i;
//				textureUnits[i] = i;
//
//				glActiveTexture(GL_TEXTURE0 + i);
//				glBindTexture(GL_TEXTURE_2D, atlasTexture->ID);
//
//				std::cout << "Binding atlas texture:\n"
//					<< "  Path: " << atlasPath << "\n"
//					<< "  Texture ID: " << atlasTexture->ID << "\n"
//					<< "  Assigned Unit: " << i << std::endl;
//
//				boundAtlases.insert(atlasPath);
//			}
//		}
//		else
//		{
//			// Handle standalone textures
//			Texture* texture = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(path).get();
//			if (!texture || texture->ID == 0)
//			{
//				std::cerr << "Error: Failed to load texture for path: " << path << std::endl;
//				continue;
//			}
//
//			// Ensure no redundant binding
//			if (textureIDMap.find(texture->ID) == textureIDMap.end())
//			{
//				textureIDMap[texture->ID] = i;
//				textureUnits[i] = i;
//
//				glActiveTexture(GL_TEXTURE0 + i);
//				glBindTexture(GL_TEXTURE_2D, texture->ID);
//
//				std::cout << "Binding standalone texture:\n"
//					<< "  Path: " << path << "\n"
//					<< "  Texture ID: " << texture->ID << "\n"
//					<< "  Assigned Unit: " << i << std::endl;
//			}
//			else
//			{
//				textureUnits[i] = textureIDMap[texture->ID];
//			}
//		}
//	}
//
//	// Pass the array of texture unit indices to the shader
//	shader->setIntArray("textures", textureUnits.data(), textureCount);
//}

/*!
 * @brief Sets up and compiles shaders used by the renderer.
 */
void Renderer::setUpShaders()
{
	shaderProgram = ECS::GetInstance().GetSystem<AssetManager>()->getShader("default");

	debug_shader_program = ECS::GetInstance().GetSystem<AssetManager>()->getShader("debug");

	UI_shader_program = ECS::GetInstance().GetSystem<AssetManager>()->getShader("UI");

	object_picking_shader_program = ECS::GetInstance().GetSystem<AssetManager>()->getShader("object_picking");

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
#ifdef _DEBUG
	renderForObjectPicking();
#endif // _DEBUG

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

		//// Set up the model matrix
		//glm::mat4 model{};
		//
		//// Copy elements from custom matrix4x4 to glm::mat4
		//for (int i = 0; i < 4; ++i)
		//	for (int j = 0; j < 4; ++j)
		//		model[i][j] = transform.transform_matrix.m2[j][i];

		// Initialize UV coordinates
		GLfloat uvCoordinates[8];
		GLint textureID = -1;

		// Check for an Animator component
		if (ECS::GetInstance().HasComponent<Animation>(entity))
		{
			auto& ani = ECS::GetInstance().GetComponent<Animation>(entity);
			auto& clip = ani.clips[ani.currentClip];
			spriteRenderer.texturePath = clip.keyPath;
			updateAnimationFrame(clip.spriteName, ani.current_frame, clip.pixel_width, clip.pixel_height, clip.total_width, clip.total_height, uvCoordinates);

			glm::vec2 pos = glm::vec2(transform.position.x, transform.position.y);
			glm::vec2 offset = glm::vec2(static_cast<float>(clip.pixel_width) * (0.5f - clip.pivot.x), static_cast<float>(clip.pixel_height) * (0.5f - clip.pivot.y));
			glm::vec2 renderPos = pos;
			if (spriteRenderer.flipX)
			{
				std::swap(uvCoordinates[0], uvCoordinates[2]); // Bottom-left <-> Bottom-right
				std::swap(uvCoordinates[1], uvCoordinates[3]);
				std::swap(uvCoordinates[4], uvCoordinates[6]); // Top-right <-> Top-left
				std::swap(uvCoordinates[5], uvCoordinates[7]);

				renderPos.x += offset.x;
				renderPos.y -= offset.y;
			}
			else
			{
				renderPos -= offset;
				//renderPos -= offset.y;
			}

			if (ECS::GetInstance().GetSystem<AssetManager>()->ifTextureExists(spriteRenderer.texturePath))
			{
				textureID = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(spriteRenderer.texturePath)->ID;
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
			finalScale.x = finalScale.y * aspectRatio;
			batchRenderer->drawSprite(glm::vec3(renderPos, transform.position.z), finalScale, glm::vec3(1.0f, 1.0f, 1.0f), mappedTextureUnit, uvCoordinates, glm::radians(transform.rotation),spriteRenderer.layer);
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

			/*if (ECS::GetInstance().GetSystem<AssetManager>()->ifTextureExists(spriteRenderer.texturePath))
			{
				textureID = ECS::GetInstance().GetSystem<AssetManager>()->getTexture(spriteRenderer.texturePath)->ID;
			}
			if (textureID < 0) {
				UME_ENGINE_WARN("Texture ID not found for {0}", spriteRenderer.texturePath);
				continue;
			}*/

			//int mappedTextureUnit = textureIDMap[textureID];

			std::string spriteName = getSpriteNameFromPath(spriteRenderer.texturePath);
			//std::cout << spriteName << " from render() line 686" <<std::endl;

			// Draw the sprite using the batch renderer, passing the updated UV coordinates
			batchRenderer->drawSprite(glm::vec3(transform.position.x, transform.position.y, transform.position.z), glm::vec2(transform.scale.x, transform.scale.y), glm::vec3(1.0f, 1.0f, 1.0f), spriteName, glm::radians(transform.rotation), spriteRenderer.layer);
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

		debugBatchRenderer->beginBatch();

		for (auto& entity : m_Entities)
		{
			auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
			auto& spriteRenderer = ECS::GetInstance().GetComponent<SpriteRender>(entity);

			// Check if the entity has a BoxCollider2D component
			if (ECS::GetInstance().HasComponent<BoxCollider2D>(entity))
			{
				auto& boxCollider = ECS::GetInstance().GetComponent<BoxCollider2D>(entity);
				// Draw box outlines for box shapes only
				if (spriteRenderer.shape == SPRITE_SHAPE::BOX)
				{
					if (boxCollider.collision_flag > 0)
						debugBatchRenderer->drawDebugBox(glm::vec3(transform.position.x, transform.position.y, 0.f), glm::vec2(transform.scale.x, transform.scale.y), glm::vec3(1.f, 0.f, 0.f), glm::radians(transform.rotation));
					else
						debugBatchRenderer->drawDebugBox(glm::vec3(transform.position.x, transform.position.y, 0.f), glm::vec2(transform.scale.x, transform.scale.y), glm::vec3(0.f, 1.f, 0.f), glm::radians(transform.rotation));
				}
			}
		}

		debugBatchRenderer->endBatch();
		debug_shader_program->Deactivate();
	}

	debug_shader_program->Activate();
	debug_shader_program->setMat4("view", view);
	debug_shader_program->setMat4("projection", projection);

#ifdef _DEBUG
	if (currentMode == InteractionMode::TRANSLATE)
		renderTranslationAxis();
	else if (currentMode == InteractionMode::ROTATE)
		renderRotationAxis();
	else if (currentMode == InteractionMode::SCALE)
		renderScaleAxis();
#endif // _DEBUG

	debug_shader_program->Deactivate();
	// Render text, UI, or additional overlays if needed
	textRenderer->renderAllText();
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
 * @param spriteName
 * @param currentFrame The current frame index.
 * @param frameWidth Width of each frame in pixels.
 * @param frameHeight Height of each frame in pixels.
 * @param totalWidth Total width of the sprite sheet.
 * @param totalHeight Total height of the sprite sheet.
 * @param uvCoordinates Output UV coordinates for the current frame.
 */
void Renderer::updateAnimationFrame(std::string spriteName, int currentFrame, int frameWidth, int frameHeight, int totalWidth, int totalHeight, GLfloat* uvCoordinates)
{
	// Calculate column and row of the current frame
	int column = currentFrame % (totalWidth / frameWidth);
	int row = currentFrame / (totalWidth / frameWidth);

	// Calculate normalized UV coordinates
	float uvX = static_cast<float>(column * frameWidth) / static_cast<float>(totalWidth);
	float uvY = static_cast<float>(row * frameHeight) / static_cast<float>(totalHeight); // Adjusted to flip vertically
	float uvWidth = static_cast<float>(frameWidth) / static_cast<float>(totalWidth);
	float uvHeight = static_cast<float>(frameHeight) / static_cast<float>(totalHeight);

	std::shared_ptr<AssetManager> amRef = ECS::GetInstance().GetSystem<AssetManager>();

	float atlasWidth = amRef->spriteData[spriteName].uv.uMax - amRef->spriteData[spriteName].uv.uMin;
	float atlasHeight = amRef->spriteData[spriteName].uv.vMax - amRef->spriteData[spriteName].uv.vMin;
	
	// Set UV coordinates with OpenGL bottom-left orientation
	uvCoordinates[0] = amRef->spriteData[spriteName].uv.uMin + uvX * atlasWidth;                // Bottom-left
	uvCoordinates[1] = amRef->spriteData[spriteName].uv.vMax - (uvY + uvHeight) * atlasHeight;
	
	uvCoordinates[2] = uvCoordinates[0] + uvWidth * atlasWidth;									// Bottom-right
	uvCoordinates[3] = amRef->spriteData[spriteName].uv.vMax - (uvY + uvHeight) * atlasHeight;
	
	uvCoordinates[4] = uvCoordinates[0] + uvWidth * atlasWidth;									// Top-right
	uvCoordinates[5] = amRef->spriteData[spriteName].uv.vMax - uvY * atlasHeight;
	
	uvCoordinates[6] = amRef->spriteData[spriteName].uv.uMin + uvX * atlasWidth;				// Top-left
	uvCoordinates[7] = amRef->spriteData[spriteName].uv.vMax - uvY * atlasHeight;
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

/*!
 * @brief Create a button object in the UI renderer.
 */
void Renderer::CreateButtonObject(const std::string& id, const Ukemochi::Vec2& position, const Ukemochi::Vec2& size, int textureID, const std::string& text, const Ukemochi::Vec3& textColor, std::string fontName, float textScale, TextAlignment alignment, bool interactable, std::function<void()> on_click)
{
	UIRenderer->addButton(UIButton(id, glm::vec2(position.x, position.y), glm::vec2(size.x, size.y), GLuint(textureID), text, glm::vec3(textColor.x, textColor.y, textColor.z), fontName, textScale, alignment, interactable, on_click));
}

/*!
 * @brief Remove a button object in the UI renderer.
 */
void Renderer::RemoveButtonObject(const std::string& id) { UIRenderer->removeButton(id); }

/*!
 * @brief Get the list of button objects in the UI renderer.
 */
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

/*!***********************************************************************
\brief
Encodes an entity ID into an RGB color vector.

\param id
The entity ID to encode.

\return
A glm::vec3 representing the RGB color corresponding to the ID.
*************************************************************************/
glm::vec3 Renderer::encodeIDToColor(int id)
{
	// This incrementation is to skip the RGB value of 0,0,0
	id += 1;

	// Separates the entity id bits into R, G and B by moving the bits out
	// Example if R wants to be encoded, we both the G and B parts out hence
	// we move it by 16 bits to the right, each color component is 8 bits
	// & 0xFF creates a comparsion between the RGB compononent in bits against 
	// 0xFF in bits
	// E.g ID = 1, comparing blue
	// 0000 0000 0000 0000 0000 0001
	// 0000 0000 0000 0000 1111 1111
	// Result:
	// 0000 0000 0000 0000 0000 0001
	// If there is a corresponding bit in the same positions, result will be 1
	float r = ((id >> 16) & 0xFF) / 255.0f;
	float g = ((id >> 8) & 0xFF) / 255.0f;
	float b = (id & 0xFF) / 255.0f;

	// Clamp the values to ensure they stay in the range [0.0, 1.0]
	return glm::vec3(glm::clamp(r, 0.0f, 1.0f),
		glm::clamp(g, 0.0f, 1.0f),
		glm::clamp(b, 0.0f, 1.0f));
}

/*!***********************************************************************
\brief
Sets up the framebuffer and associated textures for object picking.
*************************************************************************/
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

/*!***********************************************************************
\brief
Renders the scene to the object picking framebuffer using unique colors
for each entity.
*************************************************************************/
void Renderer::renderForObjectPicking()
{

	glBindFramebuffer(GL_FRAMEBUFFER, objectPickingFrameBuffer);
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Get camera matrices
	const auto& camera = ECS::GetInstance().GetSystem<Camera>();
	glm::mat4 view = camera->getCameraViewMatrix();
	glm::mat4 projection = camera->getCameraProjectionMatrix();

	object_picking_shader_program->Activate();
	object_picking_shader_program->setMat4("view", view);
	object_picking_shader_program->setMat4("projection", projection);

	colorBufferBatchRenderer->beginBatch();

	for (auto& entity : m_Entities) 
	{
		glm::vec3 color = encodeIDToColor(static_cast<int>(entity));
		auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);

		colorBufferBatchRenderer->drawDebugBox(
			glm::vec2(transform.position.x, transform.position.y),
			glm::vec2(transform.scale.x, transform.scale.y),
			color,
			glm::radians(transform.rotation)
		);
	}

	colorBufferBatchRenderer->endBatch();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*!***********************************************************************
\brief
Retrieves the entity ID corresponding to a mouse click position by
reading the color from the object picking framebuffer.

\param mouseX
The x-coordinate of the mouse click in screen space.

\param mouseY
The y-coordinate of the mouse click in screen space.

\return
The entity ID at the mouse click position, or -1 if no entity is found.
*************************************************************************/
size_t Renderer::getEntityFromMouseClick(int mouseX, int mouseY)
{
	glBindFramebuffer(GL_FRAMEBUFFER, objectPickingFrameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	unsigned char pixel[3];
	glReadPixels(mouseX, mouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Check if the color is the clear color
	if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255) 
	{
		std::cout << "No entity found at (" << mouseX << ", " << mouseY << ")" << std::endl;
		return static_cast<size_t>(-1); // Sentinel for no entity
	}
	if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) {
		std::cout << "No entity found at (" << mouseX << ", " << mouseY << ")" << std::endl;
		return static_cast<size_t>(-1); // Sentinel for no entity
	}

	if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0)
	{
		std::cout << "No entity found at (" << mouseX << ", " << mouseY << ")" << std::endl;
		return static_cast<size_t>(-1); // Sentinel for no entity
	}

	size_t entityID = ((pixel[0] << 16) | (pixel[1] << 8) | pixel[2]) - 1;

	// Handle the case where no valid entity exists (e.g., invalid ID)
	if (entityID == static_cast<size_t>(-1)) 
	{
		std::cout << "Invalid entity ID decoded at (" << mouseX << ", " << mouseY << ")" << std::endl;
		return static_cast<size_t>(-1);
	}
	// Debug output
	/*std::cout << "Mouse Click at (" << mouseX << ", " << mouseY
		<< ") " << std::endl;
	std::cout << "Pixel RGB: [" << (int)pixel[0] << ", "
		<< (int)pixel[1] << ", " << (int)pixel[2] << "]" << std::endl;
	std::cout << "Decoded Entity ID: " << entityID << std::endl;*/
	return entityID;
}

/*!***********************************************************************
\brief
Sets up the VAO, VBO, and EBO for rendering the object picking framebuffer.
*************************************************************************/
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

/*!***********************************************************************
 * @brief Draws a 2D box with the given position, dimensions, and texture,
		  starting position is the top left of screen. It starts from the
		  center of the box.
 * @param x The x-coordinate of the center of the box (in screen space).
 * @param y The y-coordinate of the center of the box (in screen space).
 * @param width The width of the box (in screen space).
 * @param height The height of the box (in screen space).
 * @param texturePath The file path to the texture for the box.
*************************************************************************/
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

/*!***********************************************************************
\brief
Retrieves the color buffer used for object picking.

\return
The OpenGL texture ID of the color buffer used for object picking.
*************************************************************************/
GLuint Renderer::getObjectPickingColorBuffer() const
{
	return colorPickingBuffer;  // this is framebuffer's color texture
}

/*!***********************************************************************
\brief
Resizes the object picking framebuffer and its associated textures
based on the new dimensions.

\param width
The new width of the framebuffer.

\param height
The new height of the framebuffer.
*************************************************************************/
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

// Optional:
// Currently here for debugging purposes only!!
// Not in use at the moment
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

/*!***********************************************************************
\brief
Handles a mouse click event for object picking and selects the entity
under the mouse cursor, if any.

\param mouseX
The x-coordinate of the mouse cursor in screen space.

\param mouseY
The y-coordinate of the mouse cursor in screen space.
*************************************************************************/
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
		selectedEntityID = static_cast<size_t>(-1); // No valid entity selected
		isDragging = false;
		std::cout << "No valid entity at mouse click position." << std::endl;
	}
}

/*!***********************************************************************
\brief
Handles dragging of the selected entity by updating its position based
on mouse movement.

\param mouseX
The x-coordinate of the mouse cursor in screen space.

\param mouseY
The y-coordinate of the mouse cursor in screen space.
*************************************************************************/
void Renderer::handleMouseDragTranslation(int mouseX, int mouseY)
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
			/*std::cout << "Dragging entity " << selectedEntityID
				<< " to position (" << transform.position.x
				<< ", " << transform.position.y << ")" << std::endl;*/
		}
		else
		{
			std::cerr << "Error: Entity " << selectedEntityID
				<< " does not have a Transform component." << std::endl;
			isDragging = false;
		}
	}
}

/*!***********************************************************************
\brief
Draws the rotation handle for the specified transform.

\param transform
The transform of the entity to draw the rotation handle for.
*************************************************************************/
void Renderer::drawRotationHandle(const Transform& transform)
{
	glm::vec3 center = glm::vec3(transform.position.x, transform.position.y, 0);

	// Circle radius based on the entity's scale
	float radius = glm::max(transform.scale.x, transform.scale.y) * 1.5f;

	// Draw the rotation circle
	debugBatchRenderer->drawDebugCircle(center, radius, glm::vec3(0, 0, 1)); // Blue circle
}

/*!***********************************************************************
\brief
Renders the rotation axis for the currently selected entity, if any.
*************************************************************************/
void Renderer::renderRotationAxis()
{

	debugBatchRenderer->beginBatch();

	// Check if an entity is selected
	if (selectedEntityID != -1 && ECS::GetInstance().HasComponent<Transform>(selectedEntityID))
	{
		auto& transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);

		// Draw rotation circle for the selected entity
		debugBatchRenderer->drawDebugCircle(
			glm::vec2(transform.position.x, transform.position.y), // Center of the entity
			glm::max(transform.scale.x, transform.scale.y) * 1.f, // Circle radius based on entity size
			glm::vec3(0.0f, 0.0f, 1.0f)                            // Circle color (blue)
		);
	}

	debugBatchRenderer->endBatch();

}

/*!***********************************************************************
\brief
Handles a mouse click event for initiating entity rotation.

\param mouseX
The x-coordinate of the mouse click in screen space.

\param mouseY
The y-coordinate of the mouse click in screen space.

\return
True if the rotation handle was clicked; otherwise, false.
*************************************************************************/
bool Renderer::handleMouseClickForRotation(int mouseX, int mouseY)
{
	if (selectedEntityID != -1)
	{
		auto& transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);
		glm::vec2 mousePosition = glm::vec2(mouseX, mouseY);
		glm::vec2 entityCenter = glm::vec2(transform.position.x, transform.position.y);
		float radius = glm::max(transform.scale.x, transform.scale.y) * 1.5f; // Circle radius

		// Calculate the distance between the mouse and the entity center
		float distance = glm::length(mousePosition - entityCenter);

		if (distance <= radius) // Mouse is within the rotation handle
		{
			isRotating = true;
			rotationStartAngle = atan2(mousePosition.y - entityCenter.y, mousePosition.x - entityCenter.x);
			rotationStartEntityAngle = glm::radians(transform.rotation);
			return true;
		}
	}
	return false;
}

/*!***********************************************************************
\brief
Handles the rotation of the selected entity based on mouse movement.

\param mouseX
The x-coordinate of the mouse cursor in screen space.

\param mouseY
The y-coordinate of the mouse cursor in screen space.
*************************************************************************/
void Renderer::handleRotation(int mouseX, int mouseY)
{
	if (isRotating)
	{
		auto& transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);
		glm::vec2 mousePosition = glm::vec2(mouseX, mouseY);
		glm::vec2 entityCenter = glm::vec2(transform.position.x, transform.position.y);

		// Calculate the current angle between the mouse and the entity center
		float currentMouseAngle = atan2(mousePosition.y - entityCenter.y, mousePosition.x - entityCenter.x);

		// Calculate the angle delta and apply it to the entity's rotation
		float angleDelta = currentMouseAngle - rotationStartAngle;
		transform.rotation = glm::degrees(rotationStartEntityAngle + angleDelta);
	}
}

/*!***********************************************************************
\brief
Handles mouse click events and delegates to the appropriate interaction mode.

\param mouseX
The x-coordinate of the mouse click in screen space.

\param mouseY
The y-coordinate of the mouse click in screen space.
*************************************************************************/
void Renderer::handleMouseClick(int mouseX, int mouseY)
{
	if (currentMode == InteractionMode::TRANSLATE)
	{
		handleMouseClickOP(mouseX, mouseY);
	}
	else if (currentMode == InteractionMode::ROTATE)
	{
		if (handleMouseClickForRotation(mouseX, mouseY))
		{
			// Do not start dragging if in rotation mode and rotation is activated
			isDragging = false;
		}
	}
	else if (currentMode == InteractionMode::SCALE)
	{
		if (handleMouseClickForScaling(mouseX, mouseY))
		{
			isDragging = false; // Scaling takes precedence over dragging
		}
	}
}

/*!***********************************************************************
\brief
Handles mouse drag events and delegates to the appropriate interaction mode.

\param mouseX
The x-coordinate of the mouse drag in screen space.

\param mouseY
The y-coordinate of the mouse drag in screen space.
*************************************************************************/
void Renderer::handleMouseDrag(int mouseX, int mouseY)
{

	if (currentMode == InteractionMode::TRANSLATE)
	{
		handleMouseDragTranslation(mouseX, mouseY);
	}
	else if (currentMode == InteractionMode::ROTATE)
	{
		handleRotation(mouseX, mouseY);
	}
	else if (currentMode == InteractionMode::SCALE)
	{
		handleScaling(mouseX, mouseY);
	}
}

/*!***********************************************************************
\brief
Renders the scale axis for the currently selected entity, if any.
*************************************************************************/
void Renderer::renderScaleAxis()
{

	debugBatchRenderer->beginBatch();

	if (selectedEntityID != -1 && ECS::GetInstance().HasComponent<Transform>(selectedEntityID))
	{
		auto& transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);

		drawScalingHandles(transform);
	}

	debugBatchRenderer->endBatch();

}

/*!***********************************************************************
\brief
Draws scaling handles for an entity to allow resizing along X, Y, or both axes.

\param transform
The transform of the entity to draw scaling handles for.
*************************************************************************/
void Renderer::drawScalingHandles(const Transform& transform)
{
	glm::vec3 entityCenter(transform.position.x, transform.position.y, 0.f);
	//glm::vec3 entityCenter(transform.position.x, transform.position.y, transform.z);

//	// Define the length of the scaling handles based on entity size
//	float handleLength = glm::max(transform.scale.x, transform.scale.y) * 0.5f;
//	// X-axis handle (red line and box)
//	glm::vec2 xHandleEnd = entityCenter + glm::vec2(handleLength, 0.0f);
//	debugBatchRenderer->drawDebugLine(entityCenter, xHandleEnd, glm::vec3(1.0f, 0.0f, 0.0f)); // Red line
//	debugBatchRenderer->drawDebugBox(xHandleEnd, glm::vec2(25.f, 25.f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0f); // Small red box
//
//	// Y-axis handle (green line and box)
//	glm::vec2 yHandleEnd = entityCenter + glm::vec2(0.0f, handleLength);
//	debugBatchRenderer->drawDebugLine(entityCenter, yHandleEnd, glm::vec3(0.0f, 1.0f, 0.0f)); // Green line
//	debugBatchRenderer->drawDebugBox(yHandleEnd, glm::vec2(25.f, 25.f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f); // Small green box
//
//	// Uniform scaling handle (center box)
//	glm::vec2 centerBoxSize(40.f, 40.f);
//	debugBatchRenderer->drawDebugBox(entityCenter, centerBoxSize, glm::vec3(0.5f, 0.5f, 0.5f), 1.0f); // Grey box

	// Define the length of the scaling handles based on entity size
	float handleLength = glm::max(transform.scale.x, transform.scale.y) * 0.5f;

	// X-axis handle (red line and box)
	glm::vec3 xHandleEnd = entityCenter + glm::vec3(handleLength, 0.0f, 0.0f);
	debugBatchRenderer->drawDebugLine(entityCenter, xHandleEnd, glm::vec3(1.0f, 0.0f, 0.0f)); // Red line
	debugBatchRenderer->drawDebugBox(xHandleEnd, glm::vec2(25.f, 25.f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0f); // Small red box

	// Y-axis handle (green line and box)
	glm::vec3 yHandleEnd = entityCenter + glm::vec3(0.0f, handleLength, 0.0f);
	debugBatchRenderer->drawDebugLine(entityCenter, yHandleEnd, glm::vec3(0.0f, 1.0f, 0.0f)); // Green line
	debugBatchRenderer->drawDebugBox(yHandleEnd, glm::vec2(25.f, 25.f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f); // Small green box

	// Uniform scaling handle (center box)
	glm::vec2 centerBoxSize(40.f, 40.f);
	debugBatchRenderer->drawDebugBox(entityCenter, centerBoxSize, glm::vec3(0.5f, 0.5f, 0.5f), 0.0f); // Grey box
}

/*!***********************************************************************
\brief
Handles a mouse click event for initiating scaling of an entity.

\param mouseX
The x-coordinate of the mouse click in screen space.

\param mouseY
The y-coordinate of the mouse click in screen space.

\return
True if the scaling handle was clicked; otherwise, false.
*************************************************************************/
bool Renderer::handleMouseClickForScaling(int mouseX, int mouseY)
{
	if (selectedEntityID != -1)
	{
		auto& transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);
		glm::vec2 mousePosition(mouseX, mouseY);
		glm::vec2 entityCenter(transform.position.x, transform.position.y);

		float handleLength = glm::max(transform.scale.x, transform.scale.y) * 0.5f;

		// Check X-axis handle
		glm::vec2 xHandleEnd = entityCenter + glm::vec2(handleLength, 0.0f);
		if (glm::length(mousePosition - xHandleEnd) <= 12.5f) // Match handle size (25.f / 2)
		{
			scalingAxis = ScalingAxis::X;
			isScaling = true;
			return true;
		}

		// Check Y-axis handle
		glm::vec2 yHandleEnd = entityCenter + glm::vec2(0.0f, handleLength);
		if (glm::length(mousePosition - yHandleEnd) <= 12.5f) // Match handle size (25.f / 2)
		{
			scalingAxis = ScalingAxis::Y;
			isScaling = true;
			return true;
		}

		// Check Uniform handle (center box)
		if (glm::length(mousePosition - entityCenter) <= 20.0f) // Match uniform box size (40.f / 2)
		{
			scalingAxis = ScalingAxis::UNIFORM;
			isScaling = true;
			return true;
		}
	}

	return false;
}

/*!***********************************************************************
\brief
Handles scaling of an entity based on mouse movement.

\param mouseX
The x-coordinate of the mouse drag in screen space.

\param mouseY
The y-coordinate of the mouse drag in screen space.
*************************************************************************/
void Renderer::handleScaling(int mouseX, int mouseY)
{
	if (isScaling && selectedEntityID != -1)
	{
		auto& transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);
		glm::vec2 mousePosition(mouseX, mouseY);
		glm::vec2 entityCenter(transform.position.x, transform.position.y);

		// Calculate the delta from the entity center
		glm::vec2 delta = mousePosition - entityCenter;

		if (scalingAxis == ScalingAxis::X)
		{
			transform.scale.x = glm::abs(delta.x); // Scale based on X-axis distance
		}
		else if (scalingAxis == ScalingAxis::Y)
		{
			transform.scale.y = glm::abs(delta.y); // Scale based on Y-axis distance
		}
		else if (scalingAxis == ScalingAxis::UNIFORM)
		{
			float uniformScale = glm::length(delta);
			transform.scale.x = uniformScale;
			transform.scale.y = uniformScale;
		}
	}
}

/*!***********************************************************************
\brief
Renders the translation axis for the currently selected entity, if any.
*************************************************************************/
void Renderer::renderTranslationAxis()
{
	if (selectedEntityID == -1 || !ECS::GetInstance().HasComponent<Transform>(selectedEntityID))
		return; // No valid entity selected

	// Get the transform component of the selected entity
	auto& transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);
	glm::vec2 entityCenter(transform.position.x, transform.position.y);

	// Define the length of the translation axis
	float axisLength = glm::max(transform.scale.x, transform.scale.y) * 0.35f;

	// Define colors for the axes
	glm::vec3 xAxisColor = glm::vec3(1.0f, 0.0f, 0.0f); // Red for X-axis
	glm::vec3 yAxisColor = glm::vec3(0.0f, 1.0f, 0.0f); // Green for Y-axis

	// Arrowhead size
	float arrowSize = axisLength * 0.1f;

	debugBatchRenderer->beginBatch();

	// Draw the X-axis
	glm::vec2 xAxisEnd = entityCenter + glm::vec2(axisLength, 0.0f);
	debugBatchRenderer->drawDebugLine(entityCenter, xAxisEnd, xAxisColor);

	// Draw arrowhead for X-axis
	glm::vec2 xArrowLeft = xAxisEnd + glm::vec2(-arrowSize, arrowSize * 0.5f);
	glm::vec2 xArrowRight = xAxisEnd + glm::vec2(-arrowSize, -arrowSize * 0.5f);
	debugBatchRenderer->drawDebugLine(xAxisEnd, xArrowLeft, xAxisColor);
	debugBatchRenderer->drawDebugLine(xAxisEnd, xArrowRight, xAxisColor);

	// Draw the Y-axis
	glm::vec2 yAxisEnd = entityCenter + glm::vec2(0.0f, axisLength);
	debugBatchRenderer->drawDebugLine(entityCenter, yAxisEnd, yAxisColor);

	// Draw arrowhead for Y-axis
	glm::vec2 yArrowLeft = yAxisEnd + glm::vec2(-arrowSize * 0.5f, -arrowSize);
	glm::vec2 yArrowRight = yAxisEnd + glm::vec2(arrowSize * 0.5f, -arrowSize);
	debugBatchRenderer->drawDebugLine(yAxisEnd, yArrowLeft, yAxisColor);
	debugBatchRenderer->drawDebugLine(yAxisEnd, yArrowRight, yAxisColor);

	debugBatchRenderer->endBatch();
}

/*!***********************************************************************
\brief
Resets the gizmo state, clearing the selected entity and interaction modes.
*************************************************************************/
void Renderer::resetGizmo()
{
	// Reset selectedEntityID when switching scenes
	selectedEntityID = static_cast<size_t>(-1);
	isScaling = false;
	isRotating = false;
}
