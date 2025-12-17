#include "particle_generator.h"
#include "gameobject.h"
#include "shader.h"
#include "texture.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iterator>

ParticleGenerator::ParticleGenerator(Shader* shader, Texture2D* texture, uint16_t amount)
    : m_shader(shader), m_texture(texture), m_amount(amount)
{
    init();
}

void ParticleGenerator::Update(float dt, GameObject& object, uint16_t new_particles, glm::vec2 offset)
{
    // respawn dead particles
    for (size_t i{}; i < new_particles; i++)
    {
        size_t dead_particle_id = getFirstDeadParticle();
        respawnParticle(m_particles.at(dead_particle_id), object, offset);
    }

    for (size_t i{}; i < m_amount; i++)
    {
        Particle& particle = m_particles.at(i);
        particle.life -= dt;
        if (particle.IsAlive()){
            particle.position -= particle.velocity * dt;
            particle.color.a -= dt * 2.5f;
        }
    }
}

void ParticleGenerator::Draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_shader->Use();
    for (Particle& particle: m_particles)
    {
        if (!particle.IsAlive()) continue;

        m_shader->SetVector2f("offset", particle.position);
        m_shader->SetVector4f("color", particle.color);
        m_texture->Bind();
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    unsigned int VBO;
    float particle_quad[]{ // vec2 (vertex), vec2 (uv)
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glBindVertexArray(0);

    for (size_t i{}; i < m_amount; i++)
    {
        m_particles.push_back(Particle{});
    }
}

size_t ParticleGenerator::getFirstDeadParticle()
{
    auto it = std::find_if(m_particles.begin(), m_particles.end(),
        [](Particle& particle) { return !particle.IsAlive(); } );
    if (it == m_particles.end()){
        return 0;
    }
    return std::distance(m_particles.begin(), it);
}

void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
    float random = ((rand() % 100) - 50) * 0.1f;
    float rand_color = 0.5f + ((rand() % 100) / 100.f);
    particle.position = object.m_position + random + offset;
    particle.color = glm::vec4(rand_color, rand_color, rand_color, 1.f);
    particle.life = 1.f;
    particle.velocity = object.m_velocity * 0.1f;
}
