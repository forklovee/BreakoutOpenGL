#pragma once

#include "gameobject.h"

class BallObject : public GameObject
{
public:
    BallObject();
    BallObject(glm::vec2 position, glm::vec2 velocity, float radius, Texture2D* sprite, glm::vec3 color = glm::vec3(1.f));

    glm::vec2 Move(float dt, unsigned int window_width);
    void Reset(glm::vec2 position, glm::vec2 velocity);

public:
    bool m_stuck;
    float m_radius;

};