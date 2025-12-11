#include "gameobject.h"

#include "sprite_renderer.h"

GameObject::GameObject()
    : m_position(0.f, 0.f), m_size(1.f, 1.f), m_rotation_deg(0.f), m_sprite(nullptr), m_color(1.f, 1.f, 1.f),
    m_is_solid(true), m_is_destroyed(false)
{

}

GameObject::GameObject(glm::vec2 position, glm::vec2 size, Texture2D* sprite, glm::vec3 color)
    : m_position(position), m_size(size), m_rotation_deg(0.f), m_sprite(sprite), m_color(color),
    m_is_solid(true), m_is_destroyed(false)
{

}

GameObject::GameObject(const GameObject& other)
    :m_position(other.m_position), m_size(other.m_size), m_rotation_deg(other.m_rotation_deg), m_sprite(other.m_sprite), m_color(other.m_color),
    m_is_solid(other.m_is_solid), m_is_destroyed(other.m_is_destroyed)
{
    
}
    
GameObject::GameObject(GameObject&& other)
    :m_position(other.m_position), m_size(other.m_size), m_rotation_deg(other.m_rotation_deg), m_sprite(other.m_sprite), m_color(other.m_color),
    m_is_solid(other.m_is_solid), m_is_destroyed(other.m_is_destroyed)
{
    other.m_sprite = nullptr;
}

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(m_sprite, m_position, m_size, m_rotation_deg, m_color);
}