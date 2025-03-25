/* Start Header ************************************************************************/
/*!
\file       Renderer.h
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu (%)
\co-authors HURNG KAI RUI, h.kairui, 2301278, h.kairui\@digipen.edu (%)
\co-authors Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu (%)
\date       Mar 12, 2025
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
#include "DebugModeBatchRendering.h"
#include "ColorBufferBatchRendering.h"
#include "../Application.h"	
#include "UIButtonManager.h"

// Forward
class TextRenderer;
class ParticleSystem;
class UIButton;

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
	 * @brief Continue to initializes the renderer by setting up the batch renderer.
	 * This function should be called once before rendering any objects.
	 */
	void batch_init();

	/*!
	 * @brief Renders all the objects (boxes and circles) set up by the renderer.
	 * This function is responsible for drawing each VAO and applying its corresponding texture.
	 */
	void render();

	void RenderMainMenuUI();


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
	static int const screen_width = 1920;
	static int const screen_height = 1080;

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

	void updateAnimationFrame(std::string spriteName, int currentFrame, int frameWidth, int frameHeight, int totalWidth, int totalHeight, GLfloat* uvCoordinates);



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
	 * @brief Update a text object color in the text renderer.
	 */
	void UpdateTextColor(const std::string& id, const glm::vec3& color);

	/*!
	 * @brief Removes the text text object.
	 */
	void RemoveTextObject(const std::string& id);

	/*!
	 * @brief Update player bars(e.g. Health, Soul, etc.).
	 */
	void updatePlayerBars();

	/*!
	 * @brief Set up framebuffer to be drawn to ImGUI.
	 */
	void setupFramebuffer();

	/*!
	 * @brief Enable framebuffer to be drawn to ImGUI.
	 */
	void beginFramebufferRender();

	/*!
	 * @brief Ends framebuffer to be drawn to ImGUI.
	 */
	void endFramebufferRender();

	/*!
	 * @brief Draws framebuffer to ImGUI.
	 */
	void renderToFramebuffer();

	GLuint getTextureColorBuffer() const;

	void resizeFramebuffer(unsigned int width, unsigned int height) const;

	EntityID playerID = (EntityID)-1;

	EntityID getPlayerID()
	{
		return playerID;
	}
public:

	//const int MAX_SOUL_CHARGES = 1;
	//const int SOUL_BAR_THRESHOLD = 5;
	/*!
	* @brief Pointer to the Shader object, which handles the OpenGL shaders.
	*/
	std::shared_ptr<Shader> shaderProgram;

	std::shared_ptr<Shader> video_shader_program;
private:
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
	int nextAvailableTextureUnit = 0;

public:
	std::unordered_map<GLuint, int> textureIDMap;
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
	 * @brief Stores animations for each entity by entity ID.
	 */
	std::unordered_map<size_t, std::vector<Animation>> entity_animations;

	bool isSlowMotion = false;  // Flag to indicate if animations are in slow motion.
	float slowMotionFactor = 2.0f;
	bool isFacingRight = false;


	
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

	GameObject* playerObject = nullptr;
	int player = -1;

public:
	std::shared_ptr<BatchRenderer2D> batchRenderer;
private:
	UIButtonManager uiManager;

	

public:
	TextRenderer* textRenderer;

	std::shared_ptr<BatchRenderer2D> batchRendererUI;
	// Setter method to set the player object
	/*void SetPlayerObject(GameObject& player)
	{
		playerObject = &player;
	}*/

	//int GetPlayer()
	//{
	//	//return Player;
	//}
	void SetPlayer(int id)
	{
		player = id;
	}

	// Not in use at the moment
	std::unique_ptr<ParticleSystem> particleSystem;
	Shader* particleShader;

	void finding_player_ID();

	void HandleInputTesting();

private:
	std::shared_ptr<Shader> UI_shader_program;


	// Object picking
private:
	/*!***********************************************************************
	\brief
	Shader program used for object picking.
	*************************************************************************/
	std::shared_ptr<Shader> object_picking_shader_program;

	/*!***********************************************************************
	\brief
	Color buffer used for object picking.
	*************************************************************************/
	GLuint colorPickingBuffer = 0;

	/*!***********************************************************************
	\brief
	Framebuffer used for object picking.
	*************************************************************************/
	GLuint objectPickingFrameBuffer = 0;

	/*!***********************************************************************
	\brief
	Renderbuffer object used for object picking depth and stencil testing.
	*************************************************************************/
	GLuint object_picking_rbo = 0;

	/*!***********************************************************************
	\brief
	Vertex Array Object for object picking.
	*************************************************************************/
	std::unique_ptr<VAO> objectPickingVAO;

	/*!***********************************************************************
	\brief
	Vertex Buffer Object for object picking geometry data.
	*************************************************************************/
	std::unique_ptr<VBO> objectPickingVBO;

	/*!***********************************************************************
	\brief
	Element Buffer Object for object picking geometry indices.
	*************************************************************************/
	std::unique_ptr<EBO> objectPickingEBO;

	/*!***********************************************************************
	\brief
	Map that associates each entity ID with a unique RGB color for object picking.
	*************************************************************************/
	std::unordered_map<size_t, glm::vec3> entityColors; // Map from entity ID to unique color

	/*!***********************************************************************
	\brief
	Sets up the framebuffer and its associated textures for object picking.
	*************************************************************************/
	void setupColorPickingFramebuffer();

	/*!***********************************************************************
	\brief
	Sets up buffers (VAO, VBO, EBO) required for object picking rendering.
	*************************************************************************/
	void setUpObjectPickingBuffer();

	/*!***********************************************************************
	\brief
	Shader program used for rendering points.
	*************************************************************************/
	std::unique_ptr<Shader> pointShader;

	/*!***********************************************************************
	\brief
	Offset between the mouse position and the entity center during dragging.
	*************************************************************************/
	glm::vec2 dragOffset = glm::vec2(0.0f, 0.0f); // Offset between mouse position and entity center

public:
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
	size_t getEntityFromMouseClick(int mouseX, int mouseY);

	/*!***********************************************************************
	\brief
	Renders all entities to the object picking framebuffer using unique colors.
	*************************************************************************/
	void renderForObjectPicking();

	/*!***********************************************************************
	\brief
	Retrieves the color buffer used for object picking.

	\return
	The OpenGL texture ID of the color buffer.
	*************************************************************************/
	GLuint getObjectPickingColorBuffer() const;

	/*!***********************************************************************
	\brief
	Resizes the object picking framebuffer and its textures based on the new dimensions.

	\param width
	The new width of the framebuffer.

	\param height
	The new height of the framebuffer.
	*************************************************************************/
	void resizeObjectPickingFramebuffer(unsigned int width, unsigned int height) const;

	/*!***********************************************************************
	\brief
	Draws a point at the specified position with the given color. (ONLY FOR DEBUGGING)

	\param x
	The x-coordinate of the point.

	\param y
	The y-coordinate of the point.

	\param color
	The color of the point (RGB).
	*************************************************************************/
	void drawPoint(float x, float y, glm::vec3 color);

	/*!***********************************************************************
	\brief
	Encodes an entity ID into a unique RGB color.

	\param id
	The entity ID to encode.

	\return
	A glm::vec3 representing the RGB color corresponding to the ID.
	*************************************************************************/
	glm::vec3 encodeIDToColor(int id);

	/*!***********************************************************************
	\brief
	Handles dragging of the selected entity based on mouse movement.

	\param mouseX
	The x-coordinate of the mouse cursor in screen space.

	\param mouseY
	The y-coordinate of the mouse cursor in screen space.
	*************************************************************************/
	void handleMouseDragTranslation(int mouseX, int mouseY);

	/*!***********************************************************************
	\brief
	Handles a mouse click event for object picking.

	\param mouseX
	The x-coordinate of the mouse click in screen space.

	\param mouseY
	The y-coordinate of the mouse click in screen space.
	*************************************************************************/
	void handleMouseClickOP(int mouseX, int mouseY);

	size_t selectedEntityID = static_cast<size_t>(-1); // Sentinel value for no selection

	/*!***********************************************************************
	\brief
	Retrieves the currently selected entity ID.

	\return
	The ID of the selected entity, or -1 if no entity is selected.
	*************************************************************************/
	size_t getSelectedEntityID() { return selectedEntityID; }

	/*!***********************************************************************
	\brief
	Flag to indicate whether dragging is currently active.
	*************************************************************************/
	bool isDragging = false; // Flag to check if dragging is active

private:
	/*!***********************************************************************
	\brief
	Handles debug rendering tasks for 2D entities.
	*************************************************************************/
	std::unique_ptr<DebugBatchRenderer2D> debugBatchRenderer;

	/*!***********************************************************************
	\brief
	Shader program used for debug rendering.
	*************************************************************************/
	std::shared_ptr<Shader> debug_shader_program;

	/*!***********************************************************************
	\brief
	Handles batch rendering for color buffer operations in 2D.
	*************************************************************************/
	std::unique_ptr<ColorBufferBatchRenderer2D> colorBufferBatchRenderer;

	// Gizmo
		// Rotation
		/*!***********************************************************************
		\brief
		Draws a rotation handle for the specified transform.

		\param transform
		The transform of the entity to draw the rotation handle for.
		*************************************************************************/
	void drawRotationHandle(const Transform& transform);

	/*!***********************************************************************
	\brief
	Renders the rotation axis for the currently selected entity.
	*************************************************************************/
	void renderRotationAxis();

	/*!***********************************************************************
	\brief
	The starting angle of rotation when a rotation operation begins.
	*************************************************************************/
	float rotationStartAngle = 0.0f;
	/*!***********************************************************************
	\brief
	The initial rotation angle of the selected entity when rotation starts.
	*************************************************************************/
	float rotationStartEntityAngle = 0.0f;

	// Scale
	/*!***********************************************************************
	\brief
	Draws scaling handles for an entity to allow resizing along X, Y, or both axes.

	\param transform
	The transform of the entity to draw scaling handles for.
	*************************************************************************/
	void drawScalingHandles(const Transform& transform);

	/*!***********************************************************************
	\brief
	Renders the scale axis for the currently selected entity.
	*************************************************************************/
	void renderScaleAxis();

	// Translation
	/*!***********************************************************************
	\brief
	Renders the translation axis for the currently selected entity.
	*************************************************************************/
	void renderTranslationAxis();

public:
	/*!***********************************************************************
	\brief
	Resets the gizmo state, clearing the selected entity and interaction modes.
	*************************************************************************/
	void resetGizmo();

	// Rotation
	/*!***********************************************************************
	\brief
	Flag to track whether a rotation operation is active.
	*************************************************************************/
	bool isRotating = false;

	/*!***********************************************************************
	\brief
	Handles a mouse click event to initiate rotation.

	\param mouseX
	The x-coordinate of the mouse click in screen space.

	\param mouseY
	The y-coordinate of the mouse click in screen space.

	\return
	True if the rotation handle was clicked; otherwise, false.
	*************************************************************************/
	bool handleMouseClickForRotation(int mouseX, int mouseY);

	/*!***********************************************************************
	\brief
	Handles rotation of the selected entity based on mouse movement.

	\param mouseX
	The x-coordinate of the mouse cursor in screen space.

	\param mouseY
	The y-coordinate of the mouse cursor in screen space.
	*************************************************************************/
	void handleRotation(int mouseX, int mouseY);

	/*!***********************************************************************
	\brief
	Enumeration representing the current interaction mode.
	*************************************************************************/
	enum class InteractionMode { TRANSLATE, ROTATE, SCALE, NO_STATE };

	/*!***********************************************************************
	\brief
	The current interaction mode (e.g., translation, rotation, scaling).
	*************************************************************************/
	InteractionMode currentMode = InteractionMode::TRANSLATE;

	/*!***********************************************************************
	\brief
	Handles a mouse click event and delegates to the appropriate interaction mode.

	\param mouseX
	The x-coordinate of the mouse click in screen space.

	\param mouseY
	The y-coordinate of the mouse click in screen space.
	*************************************************************************/
	void handleMouseClick(int mouseX, int mouseY);

	/*!***********************************************************************
	\brief
	Handles mouse drag events and delegates to the appropriate interaction mode.

	\param mouseX
	The x-coordinate of the mouse drag in screen space.

	\param mouseY
	The y-coordinate of the mouse drag in screen space.
	*************************************************************************/
	void handleMouseDrag(int mouseX, int mouseY);

	// Scale
	/*!***********************************************************************
	\brief
	Handles a mouse click event to initiate scaling.

	\param mouseX
	The x-coordinate of the mouse click in screen space.

	\param mouseY
	The y-coordinate of the mouse click in screen space.

	\return
	True if the scaling handle was clicked; otherwise, false.
	*************************************************************************/
	bool handleMouseClickForScaling(int mouseX, int mouseY);

	/*!***********************************************************************
	\brief
	Handles scaling of the selected entity based on mouse movement.

	\param mouseX
	The x-coordinate of the mouse cursor in screen space.

	\param mouseY
	The y-coordinate of the mouse cursor in screen space.
	*************************************************************************/
	void handleScaling(int mouseX, int mouseY);

	/*!***********************************************************************
	\brief
	Enumeration representing the scaling axis.

	\details
	Defines NONE for no scaling, X for horizontal scaling, Y for vertical scaling,
	and UNIFORM for uniform scaling along both axes.
	*************************************************************************/
	enum class ScalingAxis { NONE, X, Y, UNIFORM };

	/*!***********************************************************************
	\brief
	Flag to track whether a scaling operation is active.
	*************************************************************************/
	bool isScaling = false;           // Tracks whether scaling is active

	/*!***********************************************************************
	\brief
	The currently active scaling axis.
	*************************************************************************/
	ScalingAxis scalingAxis = ScalingAxis::NONE; // Tracks the active scaling axis

	bool isTransitioningToGame = false;
	float transitionTimer = 0.0f;
	const float transitionDuration = 2.0f; // duration in seconds

};
#endif
