#include "gameobject.h"

#include "sprite_renderer.h"
#include <glm/fwd.hpp>

GameObject::GameObject()
    : m_position(0.f, 0.f), m_velocity(0.f), m_size(1.f, 1.f), m_rotation_deg(0.f), m_sprite(nullptr), m_color(1.f, 1.f, 1.f),
    m_is_solid(true), m_is_destroyed(false)
{

}

GameObject::GameObject(glm::vec2 position, glm::vec2 velocity, glm::vec2 size, Texture2D* sprite, glm::vec3 color)
    : m_position(position), m_velocity(velocity), m_size(size), m_rotation_deg(0.f), m_sprite(sprite), m_color(color),
    m_is_solid(true), m_is_destroyed(false)
{

}

GameObject::GameObject(const GameObject& other)
    :m_position(other.m_position), m_velocity(other.m_velocity), m_size(other.m_size), m_rotation_deg(other.m_rotation_deg), m_sprite(other.m_sprite), m_color(other.m_color),
    m_is_solid(other.m_is_solid), m_is_destroyed(other.m_is_destroyed)
{
    
}
    
GameObject::GameObject(GameObject&& other)
    :m_position(other.m_position), m_velocity(other.m_velocity), m_size(other.m_size), m_rotation_deg(other.m_rotation_deg), m_sprite(other.m_sprite), m_color(other.m_color),
    m_is_solid(other.m_is_solid), m_is_destroyed(other.m_is_destroyed)
{
    other.m_sprite = nullptr;
}

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(m_sprite, m_position, m_size, m_rotation_deg, m_color);
}

CollisionData GameObject::CheckCollision(const GameObject& other)
{
    const bool x_axis_collision = m_position.x + m_size.x >= other.m_position.x &&
        m_position.x <= other.m_position.x + other.m_size.x;
    const bool y_axis_collision = m_position.y + m_size.y >= other.m_position.y &&
        m_position.y <= other.m_position.y + other.m_size.y;

    glm::vec2 collision_position = glm::vec2();
    glm::vec2 collision_normal = glm::vec2();

    return {
        x_axis_collision && y_axis_collision,
        collision_position,
        collision_normal,
        glm::vec2()
    };
}
