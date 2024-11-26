/* Start Header ************************************************************************/
/*!
\file       Renderer.h
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu (%)
\co-authors Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu (%)
\date       Sept 25, 2024
\brief      This file contains the declaration of the Renderer class responsible
			for handling OpenGL rendering, including setting up shaders, buffers,
			textures, and rendering 2D objects like boxes and circles.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef RENDERER_CLASS_H
#define RENDERER_CLASS_H
#define GLM_ENABLE_EXPERIMENTAL
#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../vendor/stb/stb_image.h"
#include "../vendor/glm/glm/glm.hpp"
#include <../vendor/glm/glm/gtc/matrix_transform.hpp>
#include <../vendor/glm/glm/gtc/type_ptr.hpp>
#include <../vendor/glm/glm/gtx/string_cast.hpp>
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
#include "BatchRenderer.h"

#include "../Asset Manager/AssetManager.h"
#include "UIButton.h"
#include "DebugModeBatchRendering.h"
#include "ColorBufferBatchRendering.h"

// Forward
class TextRenderer;
class ParticleSystem;
class UIButton;
class UIButtonRenderer;

struct SpriteData {
	glm::vec3 position;
	glm::vec2 size;
	float rotation;
	glm::vec4 color;
	int textureID;
};

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

	void handleMouseClick(int mouseX, int mouseY);

	/*!
	 * @brief Cleans up and releases all OpenGL resources (e.g., VAOs, VBOs, EBOs, textures, shaders).
	 */
	void cleanUp();

	/*!
	 * @brief Loads and sets up the texture for an object based on the provided file path.
	 * @param texturePath The file path to the texture to be loaded.
	 * @return The texture ID for the loaded texture.
	 */
	void setUpTextures(const std::string& texturePath, int& textureIndex);
	int current_texture_index = 0;
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

	void updateAnimationFrame(int currentFrame, int frameWidth, int frameHeight, int totalWidth, int totalHeight, GLfloat* uvCoordinates);



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

	/*!
	* @brief Create a button object in the UI renderer.
	*/
	void CreateButtonObject(const std::string& id, const Ukemochi::Vec2& position, const Ukemochi::Vec2& size, int textureID, const std::string& text, const Ukemochi::Vec3& textColor, std::string fontName, float textScale, TextAlignment alignment = TextAlignment::Center, bool interactable = true, std::function<void()> on_click = nullptr);

	std::vector<UIButton>& GetButtonObjects();

	void setupFramebuffer();

	void beginFramebufferRender();

	void endFramebufferRender();

	void renderToFramebuffer();

	GLuint getTextureColorBuffer() const;

	void resizeFramebuffer(unsigned int width, unsigned int height) const;

private:
	/*!
	* @brief Pointer to the Shader object, which handles the OpenGL shaders.
	*/
	std::shared_ptr<Shader> shaderProgram;

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
	std::vector<std::shared_ptr<Texture>> textures;

	/*!
	* @brief Texture cache to manage loaded textures
	*/
	std::unordered_map<std::string, Texture*> textureCache;
	std::vector<std::string> texturePathsOrder;
	std::unordered_map<GLuint, int> textureIDMap;
	int nextAvailableTextureUnit = 0;

public:
	/*!
	 * @brief Sets up and compiles the shaders used by the renderer.
	 */
	void setUpShaders();
private:
	/*!
	 * @brief Sets up the VAO, VBO, and EBO for an object.
	 * @param vertices Pointer to the vertex data.
	 * @param vertSize The size of the vertex data in bytes.
	 * @param indices Pointer to the index data.
	 * @param indexSize The size of the index data in bytes.
	 */
	void setUpBuffers(GLfloat* vertices, size_t vertSize, GLuint* indices, size_t indexSize);

	void bindTexturesToUnits(std::shared_ptr<Shader> shader);

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

	/*!
	 * @struct Animation
	 * @brief Manages frame-based animations for entities.
	 */
	// struct Animation {
	// 	int totalFrames;
	// 	int currentFrame;
	// 	float frameDuration;
	// 	float originalFrameDuration; // Store the original duration
	// 	float elapsedTime;
	// 	int frameWidth, frameHeight, totalWidth, totalHeight;
	// 	bool loop;
	//
	// 	/*!
	// 	 * @brief Constructs an Animation object with specified parameters.
	// 	 * @param totalFrames Total frames in the animation.
	// 	 * @param frameWidth Width of each frame.
	// 	 * @param frameHeight Height of each frame.
	// 	 * @param totalWidth Width of the sprite sheet.
	// 	 * @param totalHeight Height of the sprite sheet.
	// 	 * @param frameDuration Duration for each frame.
	// 	 * @param loop Whether the animation should loop (default is true).
	// 	 */
	// 	Animation(int totalFrames, int frameWidth, int frameHeight, int totalWidth, int totalHeight, float frameDuration, bool loop = true)
	// 		: totalFrames(totalFrames), currentFrame(0), frameDuration(frameDuration), originalFrameDuration(frameDuration),
	// 		elapsedTime(0.0f), frameWidth(frameWidth), frameHeight(frameHeight), totalWidth(totalWidth), totalHeight(totalHeight), loop(loop) {}
	//
	// 	/*!
	// 	 * @brief Updates the animation frame based on elapsed time.
	// 	 * @param deltaTime Time passed since the last update.
	// 	 */
	// 	void update(float deltaTime)
	// 	{
	// 		elapsedTime += deltaTime;
	// 		if (elapsedTime >= frameDuration) {
	//
	// 			currentFrame++;
	// 			if (currentFrame >= totalFrames) 
	// 			{
	// 				currentFrame = 0; // Loop back to the first frame
	// 			}
	// 			elapsedTime = 0.0f; // Reset elapsed time
	// 		}
	// 	}
	// 	/*!
	// 	 * @brief Sets a new duration for each frame.
	// 	 * @param newDuration New frame duration (in seconds).
	// 	 */
	// 	void setFrameDuration(float newDuration) 
	// 	{
	// 		frameDuration = newDuration;
	// 	}
	// 	/*!
	// 	 * @brief Resets the frame duration to its original value.
	// 	 */
	// 	void resetFrameDuration() 
	// 	{
	// 		frameDuration = originalFrameDuration;
	// 	}
	// };
	/*!
	 * @brief Stores animations for each entity by entity ID.
	 */
	std::unordered_map<size_t, std::vector<Animation>> entity_animations;

	bool isSlowMotion = false;  // Flag to indicate if animations are in slow motion.
	float slowMotionFactor = 2.0f;
	bool isFacingRight = false;

public:
	/*!
	 * @brief Toggles slow-motion mode for animations.
	 */
	void toggleSlowMotion();
	/*!
	 * @brief Handles animation input from the user.
	 */
	void animationKeyInput();
	/*!
	 * @brief Initializes animations for entities.
	 */
	void initAnimationEntities();

private:

	/*!
	 * @brief Initializes buffers for rendering box wireframes (debug view).
	 */
	void initDebugBoxBuffers();

	/*!
	 * @brief Initializes buffers for rendering circles.
	 * @param segments Number of segments for the circle (default is 1000).
	 */
	void initCircleBuffers(GLuint segments = 1000);

	/*!
	 * @brief Initializes buffers for rendering circle wireframes (debug view).
	 * @param segments Number of segments for the circle (default is 1000).
	 */
	void initCircleOutlineBuffers(GLuint segments = 1000);

	/*!
	 * @enum objectIDs
	 * @brief Enumerates unique identifiers for different VAOs used in rendering.
	 */
	enum objectIDs {
		BOX_OUTLINE = 0,
		CIRCLE_VAO = 1,
		CIRCLE_OUTLINE = 2,
		ANIMATION_VAO = 3
	};

	/*!
	 * @brief Framebuffer object used for off-screen rendering.
	 */
	GLuint framebuffer;

	/*!
	 * @brief Texture color buffer attached to the framebuffer.
	 */
	GLuint textureColorbuffer;

	/*!
	 * @brief Renderbuffer object for depth and stencil attachment.
	 */
	GLuint rbo;

	/*!
	 * @brief VAO for rendering the screen quad.
	 */
	std::unique_ptr<VAO> screenQuadVAO;

	/*!
	 * @brief VBO for the vertices of the screen quad.
	 */
	std::unique_ptr<VBO> screenQuadVBO;
	//std::unique_ptr<Shader> framebufferShader;

	/*!
	 * @brief Shader used for rendering the framebuffer to the screen.
	 */
	std::shared_ptr<Shader> framebufferShader;

	/*!
	 * @brief Initializes the vertex array and vertex buffer for the screen quad.
	 */
	void initScreenQuad();

	/*!
	 * @brief Renders the screen quad, typically used for post-processing effects.
	 */
	void renderScreenQuad();

	TextRenderer* textRenderer;

	GameObject* playerObject = nullptr;
	int Player = -1;

	std::shared_ptr<BatchRenderer2D> batchRenderer;

	std::unique_ptr<UIButtonRenderer> UIRenderer;

public:
	// Setter method to set the player object
	void SetPlayerObject(GameObject& player)
	{
		playerObject = &player;
	}

	int GetPlayer()
	{
		return Player;
	}
	void SetPlayer(int id)
	{
		Player = id;
	}

	// Not in use at the moment
	std::unique_ptr<ParticleSystem> particleSystem;
	Shader* particleShader;

	

private:
	std::shared_ptr<Shader> UI_shader_program;


	// Object picking
private:
	std::shared_ptr<Shader> object_picking_shader_program;
	GLuint colorPickingBuffer = 0;
	GLuint objectPickingFrameBuffer = 0;
	GLuint object_picking_rbo = 0;
	std::unique_ptr<VAO> objectPickingVAO;
	std::unique_ptr<VBO> objectPickingVBO;
	std::unique_ptr<EBO> objectPickingEBO;
	std::unordered_map<size_t, glm::vec3> entityColors; // Map from entity ID to unique color
	void assignUniqueColorsToEntities();
	void setupColorPickingFramebuffer();
	void setUpObjectPickingBuffer();
	std::unique_ptr<Shader> pointShader;

	
	glm::vec2 dragOffset = glm::vec2(0.0f, 0.0f); // Offset between mouse position and entity center

public:
	size_t getEntityFromMouseClick(int mouseX, int mouseY);
	void renderForObjectPicking();
	GLuint getObjectPickingColorBuffer() const;
	void resizeObjectPickingFramebuffer(unsigned int width, unsigned int height) const;
	void drawPoint(float x, float y, glm::vec3 color);
	glm::vec3 encodeIDToColor(int id);
	void handleMouseDrag(int mouseX, int mouseY);
	void handleMouseClickOP(int mouseX, int mouseY); 
	size_t selectedEntityID = -1; // Sentinel value for no selection
	size_t getSelectedEntityID() { return selectedEntityID; }
	bool isDragging = false; // Flag to check if dragging is active
	void renderImGuizmo();
private:
	std::unique_ptr<DebugBatchRenderer2D> debugBatchRenderer; 
	std::shared_ptr<Shader> debug_shader_program;
	
	std::unique_ptr<ColorBufferBatchRenderer2D> colorBufferBatchRenderer;
};
#endif
