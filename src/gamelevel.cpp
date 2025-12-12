#include "gamelevel.h"
#include "gameobject.h"
#include "resource_manager.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

const char* LEVELS_PATH = "../assets/levels/";

GameLevel::GameLevel()
{
    ResourceManager::LoadTexture("block.png", false, "block");
    ResourceManager::LoadTexture("block_solid.png", false, "block_solid");
}


void GameLevel::Load(const char* file, unsigned int level_width, unsigned int level_height)
{
    std::string full_path = std::string(LEVELS_PATH) + file;

    m_bricks.clear();
    unsigned int tile_code;
    std::string line;
    std::ifstream fstream(full_path.c_str());
    if (!fstream.is_open())
    {
        std::cerr << "ERROR::GAMELEVEL: Couldn't open level file '"<< full_path << "'!\n";
        return;
    }

    std::vector<std::vector<unsigned int>> tile_data;
    while(std::getline(fstream, line))
    {
        std::istringstream sstream(line);
        std::vector<unsigned int> row;
        while(sstream >> tile_code)
        {
            row.push_back(tile_code);
        }
        tile_data.push_back(row);
    }
    if (tile_data.size() > 0)
    {
        buildLevel(tile_data, level_width, level_height);
    }
}

void GameLevel::Draw(class SpriteRenderer& renderer)
{
    for (GameObject& brick: m_bricks)
    {
        if (brick.m_is_destroyed) continue;
        brick.Draw(renderer);
    }
}

bool GameLevel::IsCompleted()
{
    return std::all_of(m_bricks.begin(), m_bricks.end(),
        [](GameObject& brick){return !brick.m_is_solid && brick.m_is_destroyed;});
}

void GameLevel::buildLevel(std::vector<std::vector<unsigned int>> tile_data, unsigned int level_width, unsigned int level_height)
{
    unsigned int height = tile_data.size();
    unsigned int width = tile_data[0].size();
    float unit_width = level_width / static_cast<float>(width);
    float unit_height = level_height / static_cast<float>(height);

    for (unsigned int y{}; y < height; y++)
    {
        for (unsigned int x{}; x < width; x++)
        {
            unsigned int tile_code = tile_data[y][x];
            if (tile_code == 0) continue; // Empty

            const glm::vec2 pos(unit_width*x, unit_height*y);
            const glm::vec2 size(unit_width, unit_height);

            const bool is_solid = tile_code == 1;
            const char* texture_name = (is_solid) ? "block_solid" : "block";
            const glm::vec3 brick_color = getBrickColorByCode(tile_code);

            GameObject brick(pos, glm::vec2(0.f),
                size,
                ResourceManager::GetTexture(texture_name),
                brick_color
            );
            brick.m_is_solid = is_solid;

            m_bricks.emplace_back(std::move(brick));
        }
    }
}

glm::vec3 GameLevel::getBrickColorByCode(const unsigned int tile_code)
{
    switch(tile_code)
    {
        case 1:
            return glm::vec3(1.f);
        case 2:
            return glm::vec3(0.2f, 0.6f, 1.0f);
        case 3:
            return glm::vec3(0.0f, 0.7f, 0.0f);
        case 4:
            return glm::vec3(0.8f, 0.8f, 0.4f);
        case 5:
            return glm::vec3(1.0f, 0.5f, 0.0f);
    }
    return glm::vec3(1.f);
}
