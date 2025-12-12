#include "ball.h"
#include "gameobject.h"
#include "player.h"
#include <glm/common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

BallObject::BallObject()
    : GameObject(), m_stuck(true), m_radius(m_size.x/2.f), m_initial_velocity(m_velocity)
{

}

BallObject::BallObject(glm::vec2 position, glm::vec2 velocity, float radius, Texture2D* sprite, glm::vec3 color)
    : GameObject(position, velocity, glm::vec2(radius*2.f), sprite, color), m_stuck(true), m_radius(radius), m_initial_velocity(m_velocity)
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

CollisionData BallObject::CheckCollision(const GameObject& other)
{
    const glm::vec2 center(m_position + glm::vec2(m_radius));
    const glm::vec2 other_aabb_half_extents(other.m_size * 0.5f);
    const glm::vec2 other_aabb_center(other.m_position + other_aabb_half_extents);

    glm::vec2 diff(center - other_aabb_center);
    const glm::vec2 clamped(glm::clamp(diff, -other_aabb_half_extents, other_aabb_half_extents));
    const glm::vec2 closest(other_aabb_center + clamped);
    diff = closest - center;

    glm::vec2 normal = diff;
    if (glm::abs(normal.x) > glm::abs(normal.y))
    {
        normal.x = glm::sign(normal.x)*1.f;
        normal.y = 0.f;
    }
    else
    {
        normal.y = glm::sign(normal.y)*1.f;
        normal.x = 0.f;
    }

    return {
        glm::length(diff) <= m_radius,
        closest,
        normal,
        diff
    };
}

void BallObject::OnCollision(const CollisionData& collision)
{
    const glm::vec2 normal = collision.m_normal;
    const glm::vec2 diff = collision.m_diff;
    if (glm::abs(normal.y) > glm::abs(normal.x)) //Vertical Collision
    {
        m_velocity.y = -m_velocity.y;
        float pen = m_radius - std::abs(diff.y);
        if (glm::sign(normal.y) > 0.f)
            m_position.y -= pen; //Move up
        else
            m_position.y += pen; //Move down
    }
    else //Horizontal Collision
    {
        m_velocity.x = -m_velocity.x;
        float pen = m_radius - std::abs(diff.x);
        if (glm::sign(normal.x) > 0.f)
            m_position.x -= pen;
        else
            m_position.x += pen;
    }
}

void BallObject::OnPaddleCollision(const Player& player_paddle, const CollisionData& collision)
{
    if (m_stuck || !collision.m_collided) return;

    const float paddle_center_x = player_paddle.m_position.x + player_paddle.m_size.x * .5f;
    const float ball_center_x = m_position.x + m_radius;
    const float distance_from_center = ball_center_x - paddle_center_x;
    const float percentage = distance_from_center / (player_paddle.m_size.x*0.5f);

    const float strength = 2.0f;

    glm::vec2 last_velocity = m_velocity;
    m_velocity.x = m_initial_velocity.x * percentage * strength;
    m_velocity.y = -glm::abs(m_velocity.y);
    m_velocity = glm::normalize(m_velocity) * glm::length(last_velocity);
}



void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    m_position = position;
    m_velocity = velocity;
    m_stuck = true;
}
