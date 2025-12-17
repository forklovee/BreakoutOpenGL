#pragma once

#include <cstdint>
#include <glm/glm.hpp>

class Shader;
class Texture2D;
class GameObject;

struct Particle {
    glm::vec2 position{}, velocity{};
    glm::vec4 color{};
    float life{};

    Particle(): position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
    bool IsAlive() { return life > 0.f; }
};

class ParticleGenerator
{
public:
    ParticleGenerator(Shader* shader, Texture2D* texture, uint16_t amount);
    void Update(float dt, GameObject& object, uint16_t new_particles, glm::vec2 offset = glm::vec2(0.f, 0.f));
    void Draw();

private:
    void init();

    size_t getFirstDeadParticle();
    void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.f, 0.f));

private:
    std::vector<Particle> m_particles;
    uint16_t m_amount{};
    uint16_t m_last_used_particle_id{};

    Shader* m_shader;
    Texture2D* m_texture;
    
    unsigned int m_VAO{};

};