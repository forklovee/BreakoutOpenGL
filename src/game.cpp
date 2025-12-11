#include "game.h"

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "texture.h"
#include "window.h"

#include <iostream>
#include <memory>
#include <GL/gl.h>
#include <GLFW/glfw3.h>


std::unique_ptr<SpriteRenderer> renderer;

Game::Game(unsigned int width, unsigned int height)
    :m_width(width), m_height(height)
{
    
}

Game::~Game()
{

}

void Game::Run(){
    Window window(this, "Breakout", m_width, m_height);

    Shader* shader = ResourceManager::LoadShader(
        "shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite_shader");
    
    if (!shader) {
        std::cerr << "ERROR: sprite shader pointer is null\n";
    } else {
        std::cerr << "DEBUG: sprite shader program id = " << shader->m_id << "\n";
    }

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_width), 
        static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);
    shader->Use();
    shader->SetInteger("image", 0);
    shader->SetMatrix4("projection", projection);

    renderer = std::make_unique<SpriteRenderer>(shader);
    ResourceManager::LoadTexture("textures/trzeciaszyna.jpg", false, "cat");

    float delta_time = 0.f;
    float last_frame_time = 0.f;
    while(!window.ShouldClose())
    {
        float current_frame_time = glfwGetTime();
        delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;
        glfwPollEvents();

        ProcessInput(delta_time);
        Update(delta_time);

        window.Clear();
        Render();
        window.Present();
    }

    ResourceManager::Clear();
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


void Game::ProcessInput(float dt){
    
}

void Game::Update(float dt){
    
}

void Game::Render()
{
    Texture2D* tonfa = ResourceManager::GetTexture("cat");
    renderer->DrawSprite(tonfa, glm::vec2(0.0f, 0.0f), glm::vec2(720.0f, 720.0f));
}