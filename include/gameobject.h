#pragma once

#include <glm/glm.hpp>

class GameObject
{
public:
    GameObject();
    GameObject(glm::vec2 position, glm::vec2 size, class Texture2D* sprite, glm::vec3 color = glm::vec3(1.0f));
    
    GameObject(const GameObject& other);
    GameObject& operator=(const GameObject& other) = delete;
    
    GameObject(GameObject&& other);

    virtual void Draw(class SpriteRenderer& renderer);

public:
    glm::vec2 m_position, m_size, m_velocity;
    float m_rotation_deg;
    bool m_is_solid, m_is_destroyed;

    Texture2D* m_sprite;
    glm::vec3 m_color;
};