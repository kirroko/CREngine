#include "PreCompile.h"
#include "Particle.h"
#include "Texture.h"
#include "shaderClass.h"
#include "../ECS/Components.h"
#include "../Factory/Factory.h"

ParticleSystem::ParticleSystem(Shader* shader, Texture* texture, GLuint amount) :
    particle_shader(std::make_unique<Shader>(*shader)),
    particle_texture(std::make_unique<Texture>(*texture)),
    amount(amount),
    vbo(nullptr, 0)
{
    this->init();
}

void ParticleSystem::init()
{
    GLfloat particle_quad[] = {
        // Positions    // Texture Coords
        0.0f, 1.0f,     0.0f, 1.0f,
        1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 0.0f,     0.0f, 0.0f,

        0.0f, 1.0f,     0.0f, 1.0f,
        1.0f, 1.0f,     1.0f, 1.0f,
        1.0f, 0.0f,     1.0f, 0.0f
    };

    vao.Bind();
    vbo = VBO(particle_quad, sizeof(particle_quad));

    vao.LinkAttrib(vbo, 0, 4, GL_FLOAT, 4 * sizeof(GLfloat), (void*)0);
    vao.Unbind();
    vbo.Unbind();

    for (GLuint i = 0; i < this->amount; i++)
    {
        this->particles.push_back(Particle());
    }
}

void ParticleSystem::update(GLfloat dt, glm::vec2 position, glm::vec2 velocity, GLuint newParticles, glm::vec2 offset)
{
    // Add new particles based on newParticles count
    for (GLuint i = 0; i < newParticles; i++)
    {
        GLuint unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], position, velocity, offset);
    }

    // Update all particles
    for (GLuint i = 0; i < this->amount; i++)
    {
        Particle& p = this->particles[i];
        p.Life -= dt;
        if (p.Life > 0.0f)
        {
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5f;
        }
    }
}

void ParticleSystem::draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->particle_shader->Activate();
    for (Particle& particle : this->particles)
    {
        if (particle.Life > 0.0f)
        {
            this->particle_shader->setVec2("offset", particle.Position);
            this->particle_shader->setVec4("color", particle.Color);
            this->particle_texture->Bind();
            vao.Bind();
            glDrawArrays(GL_TRIANGLES, 0, 6);
            vao.Unbind();
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint ParticleSystem::firstUnusedParticle()
{
    static GLuint last_used_particle = 0;

    for (GLuint i = last_used_particle; i < this->amount; i++)
    {
        if (this->particles[i].Life <= 0.0f)
        {
            last_used_particle = i;
            return i;
        }
    }

    for (GLuint i = 0; i < last_used_particle; i++)
    {
        if (this->particles[i].Life <= 0.0f)
        {
            last_used_particle = i;
            return i;
        }
    }

    last_used_particle = 0;
    return 0;
}

void ParticleSystem::respawnParticle(Particle& particle, glm::vec2 position, glm::vec2 velocity, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);

    // Set the position and other properties of the particle
    particle.Position = position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = velocity * 0.1f;
}
