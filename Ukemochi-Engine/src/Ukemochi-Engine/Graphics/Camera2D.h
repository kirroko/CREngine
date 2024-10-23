#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#include "Renderer.h"
#include "../Input/Input.h"


class Camera : public Ukemochi::System
{
public:
	glm::vec2 position;
	GLfloat zoom{};
	glm::vec2 viewport_size;
	GLfloat camera_speed{};

	Camera(glm::vec2 viewport);

	glm::mat4 getCameraViewMatrix();
	glm::mat4 getCameraProjectionMatrix();
	void setViewportSize(const glm::vec2& new_size);
	void processCameraInput(GLfloat delta_time);
};
#endif // !CAMERA_CLASS_H