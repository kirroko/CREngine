#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#include "PreCompile.h"
#include "../vendor/glm/glm/glm.hpp"
#include <../vendor/glm/glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class Camera {
public:
	glm::vec2 position;
	GLfloat zoom;
	glm::vec2 viewport_size;
	GLfloat camera_speed;

	Camera(glm::vec2 viewport);

	glm::mat4 getCameraViewMatrix();
	glm::mat4 getCameraProjectionMatrix();
	void setViewportSize(const glm::vec2& new_size);

};
#endif // !CAMERA_CLASS_H
