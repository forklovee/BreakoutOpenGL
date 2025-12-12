#pragma once

#include "gameobject.h"
#include "texture.h"

class GameLevel
{
public:
    GameLevel();

    void Load(const char* file, unsigned int level_width, unsigned int level_height);
    void Draw(class SpriteRenderer& renderer);
    bool IsCompleted();

    std::vector<GameObject>& GetBricks();

private:
    void buildLevel(std::vector<std::vector<unsigned int>> tile_data, unsigned int level_width, unsigned int level_height);

    glm::vec3 getBrickColorByCode(const unsigned int tile_code);

private:
    std::vector<GameObject> m_bricks;
};