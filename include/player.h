#pragma once

#include "gameobject.h"

class Player: public GameObject
{
public:
    Player();
    Player(glm::vec2 position, glm::vec2 size, Texture2D* sprite, glm::vec3 color = glm::vec3(1.f));
    void ProcessInput(float dt, bool (&keys)[1024]);

public:
    const float PLAYER_VELOCITY = 250.f;
    unsigned int m_max_pos_x;

};