#pragma once

#include "gameobject.h"
#include "texture.h"
#include <string>

struct PowerUpContext
{
  class Player& player;
  class BallObject& ball;
  class PostProcessor& post_processor;
};

class PowerUp: public GameObject
{
public:
    enum class Type
    {
      SPEED,
      STICKY,
      PASS_THROUGH,
      PADDLE_SIZE_INCREASE,
      CONFUSE,
      CHAOS
    };

    PowerUp() = delete;
    PowerUp(Type type, glm::vec2 position);

    PowerUp(PowerUp& other);
    PowerUp& operator=(const PowerUp& other);
    
    PowerUp(PowerUp&& other) noexcept;

    void Activate(PowerUpContext& context);
    void ResetDuration();

    void Deactivate(PowerUpContext& context);

    std::string GetName() const;
    static std::string GetPowerUpName(Type type);
    static glm::vec3 GetPowerUpColor(Type type);
    static float GetPowerUpDuration(Type type);
    static Texture2D* GetPowerUpTexture(Type type);

public:
    Type m_type{};
    float m_duration{};
    bool m_activated{};
};