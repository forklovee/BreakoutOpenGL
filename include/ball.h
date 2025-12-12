#pragma once

#include "gameobject.h"

class BallObject : public GameObject
{
public:
    BallObject();
    BallObject(glm::vec2 position, glm::vec2 velocity, float radius, Texture2D* sprite, glm::vec3 color = glm::vec3(1.f));

    virtual CollisionData CheckCollision(const GameObject& other) override;
    void OnCollision(const CollisionData& collision);
    void OnPaddleCollision(const class Player& player_paddle, const CollisionData& collision);

    glm::vec2 Move(float dt, unsigned int window_width);    

    void Reset(glm::vec2 position, glm::vec2 velocity);

public:
    bool m_stuck{};
    float m_radius{};
    glm::vec2 m_initial_velocity{};

};