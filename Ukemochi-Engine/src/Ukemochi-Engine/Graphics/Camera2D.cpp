#include "PreCompile.h"
#include "Camera2D.h"

Camera::Camera() : position(0.0f, 0.0f), zoom(1.0f), viewport_size(1600.f, 900.f) {}

Camera::Camera(glm::vec2 viewportSize) : position(0.0f, 0.0f), zoom(1.0f), viewport_size(1600.f, 900.f) {}

glm::mat4 Camera::getCameraViewMatrix()
{
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));

	view = glm::scale(view, glm::vec3(zoom, zoom, 1.0f));

	return view;
}

glm::mat4 Camera::getCameraProjectionMatrix()
{
	GLfloat left = 0.0f;
	GLfloat right = viewport_size.x;
	GLfloat bottom = 0.0f;
	GLfloat top = viewport_size.y;

	return glm::ortho(left / zoom, right / zoom, bottom / zoom, top / zoom);
}

void Camera::setViewportSize(const glm::vec2& new_size)
{
	viewport_size = new_size;
}

void Camera::processCameraInput(GLfloat delta_time)
{
	GLfloat speed = camera_speed * delta_time;

	if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_UP))
		position.y += speed;
	if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_DOWN))
		position.y -= speed;
	if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_LEFT))
		position.x -= speed;
	if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_RIGHT))
		position.x += speed;

	// Handle zooming in and out with keyboard keys Q and E
	if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_Q))  // Zoom in
		zoom += zoom * delta_time;
	if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_E))  // Zoom out
		zoom -= zoom * delta_time;

	// Restrict zoom level to avoid flipping or extreme values
	zoom = glm::clamp(zoom, 0.1f, 3.0f); // Adjust as necessary

}

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
