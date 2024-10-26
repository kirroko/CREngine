/*!
 * @file    Renderer.cpp
 * @brief   This file contains the implementation of the Renderer class responsible for
			handling OpenGL rendering, including setting up shaders, buffers, textures,
			and rendering 2D objects like boxes and circles.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */
#include "PreCompile.h"
#include "Renderer.h"
#include "TextRenderer.h"

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
 * @brief Initializes the renderer, including loading shaders.
 * This function should be called once at the beginning to set up necessary resources.
 */
void Renderer::init()
{
	// Load shaders
	setUpShaders();

	// Load Buffers for box drawing
	initBoxBuffers();

	// Load Buffers for debug/wireframe box drawing
	initDebugBoxBuffers();

	// Load Buffers for circle drawing
	initCircleBuffers();

	// Load Buffers for debug/wireframe circle drawing
	initCircleOutlineBuffers();

	// Load Buffers for animation
	initAnimationBuffers();

	// Text Rendering (Test)
	// Initialize text renderer with screen dimensions
	textRenderer = new TextRenderer(screen_width, screen_height);

	// Load multiple fonts into the text renderer
	textRenderer->loadTextFont("Ukemochi", "../Assets/Fonts/Ukemochi_font-Regular.ttf");
	textRenderer->loadTextFont("Exo2", "../Assets/Fonts/Exo2-Regular.ttf");

	// Add text objects
	textRenderer->addTextObject("title", TextObject("Ukemochi!", glm::vec2(50.0f, 200.0f), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), "Ukemochi"));
	textRenderer->addTextObject("subtitle", TextObject("Exo2!", glm::vec2(50.0f, 150.0f), 1.0f, glm::vec3(0.5f, 0.8f, 0.2f), "Exo2"));

	initAnimationEntities();
	
	//particleSystem = std::make_unique<ParticleSystem>(particleShader, );
}


void Renderer::initBoxBuffers()
{
	// Define vertices for a box (centered around origin)
	GLfloat vertices_box[] = {
		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,   // Top-left
		-0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,   // Bottom-left
		 0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,   // Bottom-right
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f    // Top-right
	};
	GLuint indices_box[] = {
		0, 1, 2,
		0, 2, 3
	};

	// Set up the buffers once, and bind the VAO/VBO/EBO
	setUpBuffers(vertices_box, sizeof(vertices_box), indices_box, sizeof(indices_box));

	// Store the number of indices to be drawn
	indices_count.push_back(6);
}

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
	for (int i = 0; i <= segments; ++i)
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
	setUpBuffers(vertices.data(), vertices.size() * sizeof(GLfloat), indices.data(), indices.size() * sizeof(GLuint));
}

void Renderer::initCircleOutlineBuffers(GLuint segments)
{
	std::vector<GLfloat> vertices;

	// Define the center position (0, 0)
	GLfloat z = 0.0f;

	// Generate vertices for the circle's outline
	for (int i = 0; i <= segments; ++i) {
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

void Renderer::initAnimationBuffers()
{
	// Define the vertices with placeholder texture coordinates.
	GLfloat vertices[] = {
		// Positions         // Colors        // UVs (to be updated dynamically)
		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,   // Top-left
		-0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,   // Bottom-left
		 0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.125f, 0.0f,   // Bottom-right
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.125f, 1.0f    // Top-right
	};

	GLuint indices[] = {
		0, 1, 2,  // First triangle
		0, 2, 3   // Second triangle
	};

	setUpBuffers(vertices, sizeof(vertices), indices, sizeof(indices));
	//indices_count.push_back(6);
}


/*!
 * @brief Loads and sets up a texture based on the given file path.
 * Supports PNG (GL_RGBA) and JPG (GL_RGB) formats.
 * @param texturePath The file path to the texture to be loaded.
 */
void Renderer::setUpTextures(const std::string& texturePath)
{
	// If a texture file path is provided, load and store the texture
	if (!texturePath.empty())
	{
		// Check if the texture is already loaded in the cache and reuse existing texture
		// 0x4B45414E | We use entity spriteRenderer.texturePath as key to store the texture and retrieve honestly this is a work around not a solution
		auto it = textureCache.find(texturePath);
		if (it != textureCache.end())
		{
			//textures.push_back(it->second);
			//textures_enabled.push_back(true);
			return;
		}

		// Extract the file extension and determine format (GL_RGBA for PNG, GL_RGB for JPG)
		std::string extension = texturePath.substr(texturePath.find_last_of(".") + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		GLenum format;
		if (extension == "png")
			format = GL_RGBA;  // PNG files have transparency
		else if (extension == "jpg" || extension == "jpeg")
			format = GL_RGB;   // JPG files do not have transparency
		else
		{
			std::cerr << "Warning: Unrecognized image format. Defaulting to GL_RGB." << std::endl;
			format = GL_RGB;
		}

		// Load and store the texture with the determined format
		Texture* texture = new Texture(texturePath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, format, GL_UNSIGNED_BYTE);
		//textures.push_back(texture);
		//textures_enabled.push_back(true);
		texture->texUnit(*shaderProgram, "tex0", 0);

		textureCache[texturePath] = texture;
	}
}

/*!
 * @brief Loads and sets up the shaders to be used for rendering.
 * Currently loads vertex and fragment shaders from the specified paths.
 */
void Renderer::setUpShaders()
{
	shaderProgram = new Shader("../Assets/Shaders/default.vert", "../Assets/Shaders/default.frag");

	//particleShader = new Shader("../Assets/Shaders/particle.vert", "../Assets/Shaders/particle.frag");
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Get the current time
	GLfloat currentFrameTime = static_cast<GLfloat>(glfwGetTime());  // This will return time in seconds
	deltaTime = currentFrameTime - lastFrame;
	lastFrame = currentFrameTime;  // Save the current frame for the next iteration


	// Specify the color of the background
	glClearColor(0.07f, 0.13f, 0.17f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get the camera system instance
	auto& camera = ECS::GetInstance().GetSystem<Camera>();

	// Set up a view and projection matrix
	glm::mat4 view = camera->getCameraViewMatrix();
	glm::mat4 projection = camera->getCameraProjectionMatrix();

	// Send the projection and view matrices to the shader
	shaderProgram->Activate();
	shaderProgram->setMat4("view", view);
	shaderProgram->setMat4("projection", projection);

	GLuint entity_count = 0;
	for (auto& entity : m_Entities)
	{
		auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
		auto& spriteRenderer = ECS::GetInstance().GetComponent<SpriteRender>(entity);

		// Set vec2 to glm::vec3 for matrix transformations
		glm::vec3 position(transform.position.x, transform.position.y, 0.f);
		glm::vec3 scale(transform.scale.x, transform.scale.y, 0.f);
		// Set up the model matrix using the transform's position, scale, and rotation
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		model = glm::rotate(model, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		// Check if this object is the player object
		if (entity == playerObject->GetInstanceID() && scale_enabled) {
			// Apply scaling only to the player entity
			model = glm::scale(model, glm::vec3(transform.scale.x * scale_factor, transform.scale.y * scale_factor, 1.0f));
		}
		else {
			// Apply default scale for other entities
			model = glm::scale(model, scale);
		}

		// Apply rotation if enabled
		if (entity == playerObject->GetInstanceID() &&  rotation_enabled)
		{
			// Update the rotation angle based on deltaTime
			transform.rotation += rotationSpeed * deltaTime;
			if (transform.rotation >= 360.f)
				transform.rotation -= 360.f;

			// Apply rotation to the model matrix
			model = glm::rotate(model, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		shaderProgram->setMat4("model", model);

		// Bind the texture if available
		if (textureCache.find(spriteRenderer.texturePath) != textureCache.end()) {
			textureCache.find(spriteRenderer.texturePath)->second->Bind();  // Make sure this binds to the correct texture ID
			shaderProgram->setBool("useTexture", true);
		}
		else {
			shaderProgram->setBool("useTexture", false);
		}

		// 0x4B45414E | functions here sets up a new vertices and indices for the object
		if (spriteRenderer.animated) 
		{
			// Choose an animation based on the entity state (0 = idle, 1 = running)
			Animation& currentAnimation = entity_animations[entity][spriteRenderer.animationIndex];

			// Update and draw the current animation
			currentAnimation.update(deltaTime);
			updateAnimationFrame(currentAnimation.currentFrame,
				currentAnimation.frameWidth,
				currentAnimation.frameHeight,
				currentAnimation.totalWidth,
				currentAnimation.totalHeight);

			drawBoxAnimation();
		}
		else if (spriteRenderer.shape == SPRITE_SHAPE::BOX)
			drawBox();
		else if (spriteRenderer.shape == SPRITE_SHAPE::CIRCLE)
			drawCircle();

		if (debug_mode_enabled && spriteRenderer.shape == SPRITE_SHAPE::BOX)
			drawBoxOutline();
		else if (debug_mode_enabled && spriteRenderer.shape == SPRITE_SHAPE::CIRCLE)
			drawCircleOutline();
		entity_count++;
	}

	// Render all text objects
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
		texture.second->Delete();
		delete texture.second;
	}

	// Clear the textures vector
	textures.clear();

	// Clear the textures_enabled vector as well
	textures_enabled.clear();

	// Clear texture cache
	textureCache.clear();

	// Delete the shader program
	if (shaderProgram)
	{
		shaderProgram->Delete();
		delete shaderProgram;  // Deallocate memory
		shaderProgram = nullptr;
	}

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

void Renderer::ToggleInputsForScale()
{
	scale_enabled = static_cast<GLboolean>(!scale_enabled);
	// Adjust scale factor when toggled
	if (scale_enabled)
		scale_factor = 0.5f;
	else
		scale_factor = 1.0f;
}

void Renderer::ToggleInputsForRotation()
{
	rotation_enabled = static_cast<GLboolean>(!rotation_enabled);
	if (!rotation_enabled)
	{
		rotation_angle = 0.f;
	}
}

void Renderer::drawBoxOutline()
{
	// Bind the VAO specifically for drawing outlines
	vaos[BOX_OUTLINE]->Bind();

	// Set the line width for better visibility (optional)
	glLineWidth(2.0f);

	// Set the shader uniform to enable wireframe mode
	shaderProgram->setBool("useTexture", false);
	// Set the wireframe color to red
	shaderProgram->setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));  // Set uniform to red

	// Draw the outline using GL_LINE_LOOP to draw only the edges
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

	// Reset the uniform to disable wireframe mode for other objects
	shaderProgram->setBool("useTexture", true);

	vaos[BOX_OUTLINE]->Unbind();
}

void Renderer::drawCircleOutline()
{
	// Bind the VAO
	vaos[CIRCLE_OUTLINE]->Bind();

	// Set the shader uniform to enable wireframe mode
	shaderProgram->setBool("useTexture", false);

	// Set the wireframe color to red
	shaderProgram->setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));  // Set uniform to red

	// Draw the outline of the circle using GL_LINE_LOOP
	glDrawArrays(GL_LINE_LOOP, 0, 1000);

	// Set the shader uniform to disable wireframe mode
	shaderProgram->setBool("useTexture", true);
	// Unbind the VAO
	vaos[CIRCLE_OUTLINE]->Unbind();
}

void Renderer::updateAnimationFrame(int currentFrame, int frameWidth, int frameHeight, int totalWidth, int totalHeight)
{
	// Calculate the column and row of the current frame in the sprite sheet
	int column = currentFrame % (totalWidth / frameWidth);
	int row = currentFrame / (totalWidth / frameWidth);

	// Calculate UV coordinates based on the current column and row
	float uvX = (column * frameWidth) / static_cast<float>(totalWidth);
	float uvY = 1.0f - (row * frameHeight) / static_cast<float>(totalHeight) - (frameHeight / static_cast<float>(totalHeight)); // Flip vertically if needed
	float uvWidth = frameWidth / static_cast<float>(totalWidth);
	float uvHeight = frameHeight / static_cast<float>(totalHeight);

	// Update the UV coordinates in the VBO
	GLfloat updatedUVs[] = {
		// Positions         // Colors        // UVs
		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  uvX, uvY + uvHeight,   // Top-left
		-0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  uvX, uvY,             // Bottom-left
		 0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  uvX + uvWidth, uvY,   // Bottom-right
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  uvX + uvWidth, uvY + uvHeight    // Top-right
	};

	// Update VBO data
	vbos[ANIMATION_VAO]->UpdateData(updatedUVs, sizeof(updatedUVs));
}

void Renderer::drawBoxAnimation()
{
	// Activate the shader program and bind the texture.
	shaderProgram->Activate();

	// Draw the box using the pre-initialized VAO.
	vaos[ANIMATION_VAO]->Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	vaos[ANIMATION_VAO]->Unbind();
}

void Renderer::initAnimationEntities()
{
	int playerEntityID = 10; // Replace with actual entity IDs from your ECS or game logic

	// Create animations for the player
	Animation idleAnimation(37, 442, 448, 4096, 4096, 0.1f, true); 
	Animation runAnimation(13, 461, 428, 2048, 2048, 0.1f, true); 

	// Add multiple animations for the player entity (idle and running animations)
	entity_animations[playerEntityID] = { idleAnimation, runAnimation };

}

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
			auto& animations = entity_animations[entity];
			for (auto& animation : animations)
			{
				// Adjust the frame duration based on slow-motion state
				if (isSlowMotion)
				{
					animation.setFrameDuration(animation.originalFrameDuration * slowMotionFactor);
				}
				else
				{
					animation.resetFrameDuration(); // Reset to original duration
				}
			}
		}
	}
}

