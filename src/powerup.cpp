#include "ball.h"
#include "gameobject.h"
#include "player.h"
#include "post_processor.h"
#include "resource_manager.h"
#include "texture.h"
#include <glm/fwd.hpp>
#include <powerup.h>
#include <utility>

PowerUp::PowerUp(Type type, glm::vec2 position)
    : GameObject(position, glm::vec2(0.f, 150.f), glm::vec2(60.f, 20.f), GetPowerUpTexture(type), GetPowerUpColor(type)),
    m_type(type), m_activated(false), m_duration(GetPowerUpDuration(m_type))
{}

PowerUp::PowerUp(PowerUp& other)
    : GameObject(other), m_type(other.m_type), m_activated(other.m_activated), m_duration(other.m_duration)
{}

PowerUp& PowerUp::operator=(const PowerUp& other)
{
    GameObject::operator=(other);
    m_type = other.m_type;
    m_duration = other.m_duration;
    m_activated = other.m_activated;
    return *this;
}

PowerUp::PowerUp(PowerUp&& other) noexcept
    : GameObject(std::move(other)), m_type(other.m_type), m_activated(other.m_activated), m_duration(other.m_duration)
{
    other.m_type = Type::SPEED;
    other.m_duration = -1.f;
    other.m_activated = false;
}

void PowerUp::Activate(PowerUpContext& context)
{
  m_activated = true;

  switch(m_type)
  {
    case PowerUp::Type::SPEED:
      context.ball.m_velocity *= 1.2f;
      break;
    case PowerUp::Type::STICKY:
      context.ball.m_sticky = true;
      context.player.m_color = glm::vec3(1.f, 0.5f, 1.f);
      break;
    case PowerUp::Type::PASS_THROUGH:
      context.ball.m_pass_through = true;
      context.ball.m_color = glm::vec3(1.f, 0.5f, 0.5f);
      break;
    case PowerUp::Type::PADDLE_SIZE_INCREASE:
      context.player.m_size.x += 50;
      break;
    case PowerUp::Type::CONFUSE:
      context.post_processor.m_confuse = true;
      break;
    case PowerUp::Type::CHAOS:
      context.post_processor.m_chaos = true;
      break;
  }
}

void PowerUp::ResetDuration()
{
  m_duration = GetPowerUpDuration(m_type);
}


void PowerUp::Deactivate(PowerUpContext& context)
{
  m_is_destroyed = true;
  m_activated = true;

  switch(m_type)
  {
    case PowerUp::Type::SPEED:
      break;
    case PowerUp::Type::STICKY:
      context.ball.m_sticky = false;
      context.player.m_color = glm::vec3(1.f);
      break;
    case PowerUp::Type::PASS_THROUGH:
      context.ball.m_pass_through = false;
      context.ball.m_color = glm::vec3(1.f);
      break;
    case PowerUp::Type::PADDLE_SIZE_INCREASE:
      break;
    case PowerUp::Type::CONFUSE:
      context.post_processor.m_confuse = false;
      break;
    case PowerUp::Type::CHAOS:
      context.post_processor.m_chaos = false;
      break;
  }
}

std::string PowerUp::GetName() const
{
  return PowerUp::GetPowerUpName(m_type);
}


std::string PowerUp::GetPowerUpName(Type type)
{
    switch(type)
    {
      case Type::SPEED:
        return "speed";
      case Type::STICKY:
        return "sticky";
      case Type::PASS_THROUGH:
        return "pass through";
      case Type::PADDLE_SIZE_INCREASE:
        return "paddle size increase";
      case Type::CONFUSE:
        return "confuse";
      case Type::CHAOS:
        return "chaos";
    }
    return "none";
}

glm::vec3 PowerUp::GetPowerUpColor(Type type)
{
    switch(type)
    {
      case Type::SPEED:
        return glm::vec3(0.5f, 0.5f, 1.f);
      case Type::STICKY:
        return glm::vec3(1.f, 0.5f, 1.f);
      case Type::PASS_THROUGH:
        return glm::vec3(0.5f, 1.f, 5.f);
      case Type::PADDLE_SIZE_INCREASE:
        return glm::vec3(1.f, 0.6f, 0.4f);
      case Type::CONFUSE:
        return glm::vec3(1.f, 0.3f, 0.3f);
      case Type::CHAOS:
        return glm::vec3(0.9f, 0.25f, 0.25f);
    }
    return glm::vec3(1.f);
}

float PowerUp::GetPowerUpDuration(Type type)
{
    switch(type)
    {
      case Type::SPEED:
        return 0.0f;
      case Type::STICKY:
        return 20.f;
      case Type::PASS_THROUGH:
        return 10.f;
      case Type::PADDLE_SIZE_INCREASE:
        return 0.f;
      case Type::CONFUSE:
        return 15.f;
      case Type::CHAOS:
        return 15.f;
    }
    return 1.f;

}

Texture2D* PowerUp::GetPowerUpTexture(Type type)
{
    std::string texture_name = "";
    switch(type)
    {
    default:
        texture_name = "powerup_speed";
    case Type::SPEED:
        texture_name = "powerup_speed";
        break;
    case Type::STICKY:
        texture_name = "powerup_sticky";
        break;
    case Type::PASS_THROUGH:
        texture_name = "powerup_passthrough";
        break;
    case Type::PADDLE_SIZE_INCREASE:
        texture_name = "powerup_increase";
        break;
    case Type::CONFUSE:
        texture_name = "powerup_confuse";
        break;
    case Type::CHAOS:
        texture_name = "powerup_chaos";
        break;
    }

    Texture2D* texture = ResourceManager::GetTexture(texture_name);
    if (texture == nullptr)
    {
        std::string texture_filename = texture_name+".png";
        return ResourceManager::LoadTexture(texture_filename.c_str(), true, texture_name);
    }
    return texture;
}
