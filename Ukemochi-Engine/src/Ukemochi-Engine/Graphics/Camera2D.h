/* Start Header ************************************************************************/
/*!
\file       Camera2D.h
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Nov 6, 2024
\brief      Header file for the Camera class, which provides 2D camera functionalities for panning, zooming, and viewport management.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../vendor/glm/glm/glm.hpp"
#include <../vendor/glm/glm/gtc/matrix_transform.hpp>
#include "../Input/Input.h"
#include "../ECS/Systems.h"

/*!
 * @class Camera
 * @brief A 2D camera class that manages position, zoom, and viewport, allowing for transformations and user input handling.
 */
class Camera : public Ukemochi::System
{
public:
	glm::vec2 position;
	GLfloat zoom = 1.f;
	glm::vec2 viewport_size;
	GLfloat camera_speed = 1000.f;

	/*!
	 * @brief Default constructor initializing camera with default settings.
	 */
	Camera();
	/*!
	 * @brief Constructor initializing the camera with a specific viewport size.
	 * @param viewport The initial size of the camera's viewport.
	 */
	Camera(glm::vec2 viewport);

	/*!
	 * @brief Gets the view matrix for the camera, based on position and zoom.
	 * @return A 4x4 view matrix.
	 */
	glm::mat4 getCameraViewMatrix();

	/*!
	 * @brief Gets the orthographic projection matrix for the camera.
	 * @return A 4x4 projection matrix.
	 */
	glm::mat4 getCameraProjectionMatrix();

	/*!
	 * @brief Sets the size of the camera's viewport.
	 * @param new_size The new viewport size.
	 */
	void setViewportSize(const glm::vec2& new_size);

	/*!
	* @brief Processes input for moving and zooming the camera.
	* @param delta_time Time elapsed since the last frame, used to scale movement.
	*/
	void processCameraInput(GLfloat delta_time);

	/*!
	 * @brief Zooms the camera towards the cursor position.
	 * @param zoomFactor The factor by which to zoom in or out.
	 */
	void ZoomTowardsCursor(float zoomFactor);
};
#endif // !CAMERA_CLASS_H