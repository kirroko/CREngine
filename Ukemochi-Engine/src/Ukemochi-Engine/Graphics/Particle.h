#ifndef PARTICLE_CLASS_H 
#define PARTICLE_CLASS_H 
#include "../vendor/glm/glm/glm.hpp" 
#include <glad/glad.h> 
#include "VAO.h" 
#include "VBO.h" 
#include <memory>

class Texture;
class Shader;
class VAO;
class VBO;

class Particle {
public:
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;
};

class ParticleSystem {
public:
	ParticleSystem(Shader* Shader, Texture* Texture, GLuint amount);

	void update(GLfloat dt, glm::vec2 position, glm::vec2 velocity, GLuint newParticles, glm::vec2 offset);
	void draw();

private:
	std::vector<Particle> particles;
	GLuint amount;
	std::unique_ptr<Shader> particle_shader;
	std::unique_ptr<Texture> particle_texture;
	VAO vao;
	VBO vbo;

	void init();
	GLuint firstUnusedParticle();
	void respawnParticle(Particle& particle, glm::vec2 position, glm::vec2 velocity, glm::vec2 offset);

};
#endif // !PARTICLE_CLASS_H