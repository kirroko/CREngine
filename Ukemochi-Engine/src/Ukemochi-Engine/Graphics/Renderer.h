/*!
 * @file    Renderer.h
 * @brief   This file contains the declaration of the Renderer class responsible
			for handling OpenGL rendering, including setting up shaders, buffers,
			textures, and rendering 2D objects like boxes and circles.
 * @author  t.shunzhitomy@digipen.edu
 * @date    25/09/2024
 */
#ifndef RENDERER_CLASS_H
#define RENDERER_CLASS_H
#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../vendor/stb/stb_image.h"
#include "../vendor/glm/glm/glm.hpp"
#include <../vendor/glm/glm/gtc/matrix_transform.hpp>
#include <../vendor/glm/glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Particle.h"
#include "Ukemochi-Engine/ECS/ECS.h"
#include "Camera2D.h"
#include "Ukemochi-Engine/Factory/GameObject.h"
#include "Ukemochi-Engine/ECS/Entity.h"


// Froward
class TextRenderer;
class ParticleSystem;

using namespace Ukemochi;
 /*!
  * @class Renderer
  * @brief A class that manages OpenGL rendering, shader setup, texture handling, and rendering 2D objects like boxes and circles.
  */
class Renderer : public Ukemochi::System
{
public:
	/*!
	 * @brief Constructor for the Renderer class.
	 * Initializes OpenGL objects and prepares the renderer.
	 */
	Renderer();
	/*!
	* @brief Destructor for the Renderer class.
	* Cleans up all allocated OpenGL resources.
	*/
	~Renderer();

	/*!
	 * @brief Initializes the renderer by setting up necessary resources, such as shaders.
	 * This function should be called once before rendering any objects.
	 */
	void init();

	/*!
	 * @brief Renders all the objects (boxes and circles) set up by the renderer.
	 * This function is responsible for drawing each VAO and applying its corresponding texture.
	 */
	void render();

	/*!
	 * @brief Cleans up and releases all OpenGL resources (e.g., VAOs, VBOs, EBOs, textures, shaders).
	 */
	void cleanUp();

	/*!
	 * @brief Loads and sets up the texture for an object based on the provided file path.
	 * @param texturePath The file path to the texture to be loaded.
	 * @return The texture ID for the loaded texture.
	 */
	void setUpTextures(const std::string& texturePath);

	static int const screen_width = 1600;
	static int const screen_height = 900;

	/*!

	 */
	void drawBox();

	/*!
	 * @brief Draws a 2D circle at the specified position with the given radius and optional texture.
	 * @param x The x-coordinate of the center of the circle (in screen space).
	 * @param y The y-coordinate of the center of the circle (in screen space).
	 * @param radius The radius of the circle (in screen space).
	 */
	void drawCircle();

	/*!
	 * @brief Toggles scale transformation for objects based on input.
	 */
	void ToggleInputsForScale();

	/*!
	 * @brief Toggles rotation transformation for objects based on input.
	 */
	void ToggleInputsForRotation();


	/*!
	* @brief Draws the outline of a 2D box at the specified position.
	*/
	void drawBoxOutline();
	/*!
	 * @brief Draws the outline of a 2D circle at the specified position.
	 */
	void drawCircleOutline();

	void updateAnimationFrame(int currentFrame, int frameWidth, int frameHeight, int totalWidth, int totalHeight);


	void drawBoxAnimation();

	/*!
	 * @brief Debug mode flag to enable drawing of object outlines.
	 */
	GLboolean debug_mode_enabled = false;

	/*!
	* @brief Create a text object in the text renderer.
	*/
	void CreateTextObject(const std::string& id, const std::string& label, const Ukemochi::Vec2& pos, const float scale, const Ukemochi::Vec3& color, const std::string& font_name);

	/*!
	* @brief Update a text object in the text renderer.
	*/
	void UpdateTextObject(const std::string& id, const std::string& newText);
	
	void setupFramebuffer();

	void beginFramebufferRender();

	void endFramebufferRender();

	void renderToFramebuffer();

	GLuint getTextureColorBuffer() const;

	void resizeFramebuffer(int width, int height);

private:
	/*!
	* @brief Pointer to the Shader object, which handles the OpenGL shaders.
	*/
	Shader* shaderProgram;

	/*!
	 * @brief Vector storing the pointers to VAOs (Vertex Array Objects) used for rendering.
	 */
	std::vector<VAO*> vaos;

	/*!
	* @brief Vector storing the pointers to VBOs (Vertex Buffer Objects) used for rendering.
	*/
	std::vector<VBO*> vbos;

	/*!
	 * @brief Vector storing the pointers to EBOs (Element Buffer Objects) used for rendering.
	 */
	std::vector<EBO*> ebos;

	/*!
	 * @brief Vector storing the number of indices for each object, used for rendering with glDrawElements().
	 */
	std::vector<size_t> indices_count;

	/*!
	 * @brief Vector storing boolean flags indicating whether a texture is enabled for each object.
	 */
	std::vector<GLboolean> textures_enabled;

	/*!
	 * @brief Vector storing pointers to Texture objects used by the renderer.
	 */
	std::vector<Texture*> textures;

	/*!
	* @brief Texture cache to manage loaded textures
	*/
	std::unordered_map<std::string, Texture*> textureCache;

	/*!
	 * @brief Sets up and compiles the shaders used by the renderer.
	 */
	void setUpShaders();

	/*!
	 * @brief Sets up the VAO, VBO, and EBO for an object.
	 * @param vertices Pointer to the vertex data.
	 * @param vertSize The size of the vertex data in bytes.
	 * @param indices Pointer to the index data.
	 * @param indexSize The size of the index data in bytes.
	 */
	void setUpBuffers(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indexSize);

	/*!
	 * @brief Scale factor applied to objects when scaling is enabled.
	 */
	GLfloat scale_factor{};

	/*!
	 * @brief Flag to enable or disable scaling of objects.
	 */
	GLboolean scale_enabled = false;

	/*! @brief Flag to enable or disable rotation of objects. */
	GLboolean rotation_enabled = false;

	/*!
	 * @brief Angle for rotating objects (in degrees).
	 */
	GLfloat rotation_angle{};

	/*!
	 * @brief Speed at which objects rotate (degrees per second).
	 */
	GLfloat rotationSpeed = 45.0f;

	/*!
	 * @brief Time elapsed between the current and previous frame.
	 */
	GLfloat deltaTime = 0.0f;

	/*!
	 * @brief Time recorded for the previous frame, used for calculating deltaTime.
	 */
	GLfloat lastFrame = 0.0f;

	// Animation control
	struct Animation {
		int totalFrames;
		int currentFrame;
		float frameDuration;
		float originalFrameDuration; // Store the original duration
		float elapsedTime;
		int frameWidth, frameHeight, totalWidth, totalHeight;
		bool loop;

		Animation(int totalFrames, int frameWidth, int frameHeight, int totalWidth, int totalHeight, float frameDuration, bool loop = true)
			: totalFrames(totalFrames), currentFrame(0), frameDuration(frameDuration), originalFrameDuration(frameDuration),
			elapsedTime(0.0f), frameWidth(frameWidth), frameHeight(frameHeight), totalWidth(totalWidth), totalHeight(totalHeight), loop(loop) {}

		void update(float deltaTime)
		{
			elapsedTime += deltaTime;
			if (elapsedTime >= frameDuration) {

				currentFrame++;
				if (currentFrame >= totalFrames) 
				{
					currentFrame = 0; // Loop back to the first frame
				}
				elapsedTime = 0.0f; // Reset elapsed time
			}
		}

		void setFrameDuration(float newDuration) 
		{
			frameDuration = newDuration;
		}

		void resetFrameDuration() 
		{
			frameDuration = originalFrameDuration;
		}
	};
	std::unordered_map<int, std::vector<Animation>> entity_animations;

	bool isSlowMotion = false;
	float slowMotionFactor = 2.0f;
	bool isFacingRight = false;

public:
	void toggleSlowMotion();
	void animationKeyInput();
	void initAnimationEntities();

private:


	void initBoxBuffers();

	void initDebugBoxBuffers();

	void initCircleBuffers(GLuint segments = 1000);

	void initCircleOutlineBuffers(GLuint segments = 1000);

	void initAnimationBuffers();

	enum objectIDs {
		BOX_VAO = 0,
		BOX_OUTLINE = 1,
		CIRCLE_VAO = 2,
		CIRCLE_OUTLINE = 3,
		ANIMATION_VAO = 4
	};

	GLuint framebuffer;

	GLuint textureColorbuffer;

	GLuint rbo;
	std::unique_ptr<VAO> screenQuadVAO;
	std::unique_ptr<VBO> screenQuadVBO;
	std::unique_ptr<Shader> framebufferShader;
	void initScreenQuad();
	void renderScreenQuad();


	
	TextRenderer* textRenderer;

	GameObject* playerObject = nullptr;
	EntityID Player = -1;

public:
	// Setter method to set the player object
	void SetPlayerObject(GameObject& player) 
	{
			playerObject = &player;
	}

	EntityID GetPlayer()
	{
		return Player;
	}
	void SetPlayer(EntityID id)
	{
		Player = id;
	}

	std::unique_ptr<ParticleSystem> particleSystem;
	Shader* particleShader;

};
#endif
