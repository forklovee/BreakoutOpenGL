#pragma once

#include "gameobject.h"
#include <cstdint>

class Player: public GameObject
{
public:
    Player();
    Player(glm::vec2 position, glm::vec2 velocity, glm::vec2 size, Texture2D* sprite, glm::vec3 color = glm::vec3(1.f));
    void ProcessInput(float dt, const std::array<bool, 1024>& key_states);

    glm::vec2 GetBallSlotPosition(GameObject& ball) const;

    void RenewLifePoints(uint8_t new_life_points);
    void LoseLifePoint();
    bool HasLifePoints() const;

public:
    const float PLAYER_VELOCITY = 250.f;
    unsigned int m_max_pos_x{};

    uint8_t m_life_points{};

};