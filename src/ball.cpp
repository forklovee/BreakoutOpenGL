#include "ball.h"
#include "gameobject.h"
#include <glm/fwd.hpp>

BallObject::BallObject()
    : GameObject(), m_stuck(true), m_radius(m_size.x/2.f)
{

}

BallObject::BallObject(glm::vec2 position, glm::vec2 velocity, float radius, Texture2D* sprite, glm::vec3 color)
    : GameObject(position, velocity, glm::vec2(radius*.5f), sprite, color), m_stuck(true), m_radius(radius)
{

}

glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
    if (m_stuck){
        return m_position;
    }

    m_position += m_velocity * dt;
    // X movement
    if (m_position.x <= 0.f)
    {
        m_velocity.x = -m_velocity.x;
        m_position.x = 0.f;
    }
    else if (m_position.x + m_size.x >= window_width)
    {
        m_velocity.x = -m_velocity.x;
        m_position.x = window_width - m_size.x;
    }
    // Y movement
    if (m_position.y <= 0.f)
    {
        m_velocity.y = -m_velocity.y;
        m_position.y = 0.f;
    }

    return m_position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    m_position = position;
    m_velocity = velocity;
    m_stuck = true;
}
