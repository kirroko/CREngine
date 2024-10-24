#ifndef PARTICLE_CLASS_H
#define PARTICLE_CLASS_H
#include "PreCompile.h"
#include "../vendor/glm/glm/glm.hpp"
#include <glad/glad.h>
#include "shaderClass.h"
class Particle {
public:
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;
};

class ParticleSytem {
public:

};
#endif // !PARTICLE_CLASS_H
