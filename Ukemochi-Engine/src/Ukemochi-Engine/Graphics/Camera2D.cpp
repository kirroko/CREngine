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

	return glm::ortho(left, right, bottom, top);
}

void Camera::setViewportSize(const glm::vec2& new_size)
{
	viewport_size = new_size;
}

void Camera::processCameraInput(GLfloat delta_time)
{
	GLfloat speed = camera_speed * delta_time;

	if (UME::Input::IsKeyPressed(GLFW_KEY_UP))
		position.y += speed;
	if (UME::Input::IsKeyPressed(GLFW_KEY_DOWN))
		position.y -= speed;
	if (UME::Input::IsKeyPressed(GLFW_KEY_LEFT))
		position.x -= speed;
	if (UME::Input::IsKeyPressed(GLFW_KEY_RIGHT))
		position.x += speed;
}
