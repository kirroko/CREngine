#ifndef PARTICLE_CLASS_H
#define PARTICLE_CLASS_H
#include "PreCompile.h"
#include "../vendor/glm/glm/glm.hpp"
#include <glad/glad.h>

class Particle {
public:
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {};

    // Method to update the particle
    void Update(GLfloat dt) 
    {
        Life -= dt;
        if (Life > 0.0f) 
        {
            Position += Velocity * dt; // Update position based on velocity
            Color.a -= dt * 2.5f; // Fade particle out over time
        }
    }

    bool IsAlive() const 
    {
        return Life > 0.0f;
    }

};

class ParticleSystem {
public:
    std::vector<Particle> particles;
    GLuint VAO, VBO;

    ParticleSystem(GLuint maxParticles)
    {
        particles.resize(maxParticles);
        initRenderData();
    }

    void update(GLfloat dt, const glm::vec2& playerPosition)
    {
        for (auto& particle : particles)
        {
            if (!particle.IsAlive())
                respawnParticle(particle, playerPosition);

            particle.Update(dt);
        }
    }

    void draw()
    {
        glBindVertexArray(VAO);
        for (const auto& particle : particles)
        {
            if (particle.IsAlive())
            {
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        glBindVertexArray(0);
    }

private:

    void initRenderData()
    {
        GLfloat particleQuad[] = {
            // Positions   // Texture Coords
            -0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

        glBindVertexArray(0);
    }

    void respawnParticle(Particle& particle, const glm::vec2& playerPosition) 
    {
        float random = ((rand() % 100) - 50) / 10.0f;
        float rColor = 0.5f + ((rand() % 100) / 100.0f); 

        particle.Position = playerPosition + glm::vec2(random, random);
        particle.Velocity = glm::vec2(0.0f, 1.0f);
        particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
        particle.Life = 1.0f;
    }

};
#endif // !PARTICLE_CLASS_H
