#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../vendor/glm/glm/glm.hpp"
#include <../vendor/glm/glm/gtc/matrix_transform.hpp>
#include "../Input/Input.h"
#include "../ECS/Systems.h"

class Camera : public Ukemochi::System
{
public:
	glm::vec2 position;
	GLfloat zoom = 1.f;
	glm::vec2 viewport_size;
	GLfloat camera_speed = 1000.f;

	Camera();
	Camera(glm::vec2 viewport);

	glm::mat4 getCameraViewMatrix();
	glm::mat4 getCameraProjectionMatrix();
	void setViewportSize(const glm::vec2& new_size);
	void processCameraInput(GLfloat delta_time);
	void ZoomTowardsCursor(float zoomFactor);
};
#endif // !CAMERA_CLASS_H