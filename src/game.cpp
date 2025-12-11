#include "game.h"
#include "resource_manager.h"
#include "window.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>

Game::Game(unsigned int width, unsigned int height)
    :m_width(width), m_height(height)
{
    
}

Game::~Game(){

}

void Game::Run(){
    Window window(this, "Breakout", m_width, m_height);

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

        Render();

        window.Refresh();
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


void Game::ProcessInput(float dt){
    
}

void Game::Update(float dt){
    
}

void Game::Render(){
    
}