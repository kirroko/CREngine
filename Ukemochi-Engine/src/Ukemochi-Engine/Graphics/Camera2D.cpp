/* Start Header ************************************************************************/
/*!
\file       Camera2D.cpp
\author     TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu
\date       Nov 6, 2024
\brief      Implementation of the Camera class for managing a 2D camera in a game or application.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Camera2D.h"

 /*!
  * @brief Default constructor initializing the camera at origin with default zoom and viewport size.
  */
Camera::Camera() : position(0.0f, 0.0f), zoom(1.0f), viewport_size(1920.f, 1080.f) {}
/*!
 * @brief Constructs a Camera object with a specified viewport size.
 * @param viewportSize Initial size of the viewport.
 */
Camera::Camera(glm::vec2 viewportSize) : position(0.0f, 0.0f), zoom(1.0f), viewport_size(1920.f, 1080.f) 
{
	(void)viewportSize;
}

/*!
 * @brief Computes the view matrix based on the camera's position and zoom level.
 * @return A 4x4 matrix representing the camera's view transformation.
 */
glm::mat4 Camera::getCameraViewMatrix()
{
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));

	view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));

	return view;
}

/*!
 * @brief Computes an orthographic projection matrix for the camera.
 * @return A 4x4 orthographic projection matrix.
 */
glm::mat4 Camera::getCameraProjectionMatrix()
{
	GLfloat left = 0.f;
	GLfloat right = viewport_size.x;
	GLfloat bottom = 0.f;
	GLfloat top = viewport_size.y;

	return glm::ortho(left, right , bottom, top);
}

/*!
 * @brief Sets the viewport size of the camera, used for projection calculations.
 * @param new_size The new dimensions for the viewport.
 */
void Camera::setViewportSize(const glm::vec2& new_size)
{
	viewport_size = new_size;
}

/*!
 * @brief Processes keyboard input to move and zoom the camera, updating its position and zoom based on user input.
 * @param delta_time Time elapsed since the last frame, affecting movement speed.
 */
void Camera::processCameraInput(GLfloat delta_time)
{
	(void)delta_time;
#ifdef _DEBUG
	GLfloat speed = camera_speed * delta_time;

	// Handle zooming in and out with keyboard keys Q and E
	if (Ukemochi::Input::IsKeyPressed(UME_KEY_UP))
		position.y += speed;
	if (Ukemochi::Input::IsKeyPressed(UME_KEY_DOWN))
		position.y -= speed;
	if (Ukemochi::Input::IsKeyPressed(UME_KEY_LEFT))
		position.x -= speed;
	if (Ukemochi::Input::IsKeyPressed(UME_KEY_RIGHT))
		position.x += speed;

	//if (Ukemochi::Input::IsKeyPressed(UME_KEY_Q))  // Zoom in
	//	zoom += zoom * delta_time;
	//if (Ukemochi::Input::IsKeyPressed(UME_KEY_E))  // Zoom out
	//	zoom -= zoom * delta_time;

	// Restrict zoom level to avoid flipping or extreme values
	zoom = glm::clamp(zoom, 0.1f, 3.0f); // Adjust as necessary
#endif // _DEBUG
}

/*!
 * @brief Zooms the camera towards the cursor, adjusting the position to keep the cursor's world position constant.
 * @param zoomFactor The factor by which to zoom.
 */
void Camera::ZoomTowardsCursor(float zoomFactor)
{
	// Get current mouse position in screen coordinates
	auto [mouseX, mouseY] = Ukemochi::Input::GetMousePosition();

	// Normalize mouse position in screen space to range [-1, 1]
	glm::vec2 normalizedMousePos = {
		(mouseX / viewport_size.x) * 2.0f - 1.0f,
		1.0f - (mouseY / viewport_size.y) * 2.0f // Flip Y coordinate
	};

	// Calculate the current world position of the cursor
	glm::vec4 mouseWorldPos = glm::inverse(getCameraProjectionMatrix() * getCameraViewMatrix()) * glm::vec4(normalizedMousePos, 0.0f, 1.0f);

	// Update zoom level
	zoom *= zoomFactor;

	// Calculate new world position of the cursor after zoom change
	glm::vec4 newMouseWorldPos = glm::inverse(getCameraProjectionMatrix() * getCameraViewMatrix()) * glm::vec4(normalizedMousePos, 0.0f, 1.0f);

	// Calculate the difference and adjust camera position
	glm::vec4 delta = newMouseWorldPos - mouseWorldPos;
	position.x -= delta.x;
	position.y -= delta.y;

	// Restrict zoom level to avoid flipping or extreme values
	zoom = glm::clamp(zoom, 0.1f, 3.0f);
}
