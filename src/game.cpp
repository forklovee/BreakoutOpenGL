#include "game.h"

#include "gamelevel.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "texture.h"
#include "window.h"

#include <iostream>
#include <memory>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <utility>


std::unique_ptr<SpriteRenderer> renderer;

Game::~Game()
{
    ResourceManager::Clear();
}

void Game::Run(unsigned int screen_width, unsigned int screen_height){
    m_state = GAME_ACTIVE;

    m_window = std::make_unique<Window>(this, "Breakout", screen_width, screen_height);
    init();

    float delta_time = 0.f;
    float last_frame_time = 0.f;
    while(!m_window->ShouldClose())
    {
        float current_frame_time = glfwGetTime();
        delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;
        glfwPollEvents();

        processInput(delta_time);
        update(delta_time);

        m_window->Clear();
        render();
        m_window->Present();
    }
}

void Game::OnKeyActionCallback(Window* window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            m_keys[key] = true;
        else if (action == GLFW_RELEASE)
            m_keys[key] = false;
    }
}

void Game::OnWindowResized(Window* window, int width, int height)
{
    std::cout << "Game::OnWindowResized: ("<<width<<","<<height<<")\n";
}

void Game::init()
{
    Shader* shader = ResourceManager::LoadShader(
        "sprite.vs", "sprite.frag", nullptr, "sprite_shader");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_window->m_width), 
        static_cast<float>(m_window->m_height), 0.0f, -1.0f, 1.0f);
    shader->Use();
    shader->SetInteger("image", 0);
    shader->SetMatrix4("projection", projection);

    renderer = std::make_unique<SpriteRenderer>(shader);
    ResourceManager::LoadTexture("trzeciaszyna.jpg", false, "tonfa");
    ResourceManager::LoadTexture("funnycat.jpg", false, "cat");

    GameLevel first;
    first.Load("standard.lvl", m_window->m_width, m_window->m_height/2);
    m_levels.emplace_back(std::move(first));
    GameLevel second;
    second.Load("gaps.lvl", m_window->m_width, m_window->m_height/2);
    m_levels.emplace_back(std::move(second));
    GameLevel third;
    third.Load("invader.lvl", m_window->m_width, m_window->m_height/2);
    m_levels.emplace_back(std::move(third));
    GameLevel fourth;
    fourth.Load("bounce.lvl", m_window->m_width, m_window->m_height/2);
    m_levels.emplace_back(std::move(fourth));
    m_current_level = 0;
}


void Game::processInput(float dt){
    
}

void Game::update(float dt){
    
}

void Game::render()
{
    Texture2D* tonfa = ResourceManager::GetTexture("tonfa");
    Texture2D* funnycat = ResourceManager::GetTexture("cat");
    renderer->DrawSprite(tonfa, glm::vec2(0.0f, 0.0f), glm::vec2(720.0f, 720.0f));
    renderer->DrawSprite(tonfa, glm::vec2(55.0f, 0.0f), glm::vec2(100.0f, 100.0f));
    renderer->DrawSprite(funnycat, glm::vec2(120.0f, 0.0f), glm::vec2(320.0f, 180.0f));
    switch(m_state)
    {
        case GAME_ACTIVE:
            GameLevel& current_level = m_levels.at(m_current_level);
            current_level.Draw(*renderer.get());
            break;
    }
}