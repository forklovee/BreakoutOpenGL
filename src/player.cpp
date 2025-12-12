#include "player.h"
#include "gameobject.h"
#include <GLFW/glfw3.h>

Player::Player()
    : GameObject(), m_max_pos_x(100)
{
    
}

Player::Player(glm::vec2 position, glm::vec2 velocity, glm::vec2 size, Texture2D* sprite, glm::vec3 color)
    : GameObject(position, velocity, size, sprite, color), m_max_pos_x(100)
{

}

void Player::ProcessInput(float dt, const std::array<bool, 1024>& key_states)
{
    m_velocity.x = PLAYER_VELOCITY * dt;

    if (key_states[GLFW_KEY_A] && m_position.x >= 0.0f)
    {
        m_position.x -= m_velocity.x;
    }

    if (key_states[GLFW_KEY_D] && m_position.x <= m_max_pos_x - m_size.x)
    {
        m_position.x += m_velocity.x;
    }
}
