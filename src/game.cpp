#include "game.h"

#include "ball.h"
#include "gamelevel.h"
#include "gameobject.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "texture.h"
#include "window.h"

#include <glm/common.hpp>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <utility>


std::unique_ptr<SpriteRenderer> renderer;

Game::Game()
    : m_key_states{}
{

}

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
        processCollisions();

        m_window->Clear();
        render();
        m_window->Present();
    }
}

GameLevel& Game::GetCurrentLevel()
{
    return m_levels[m_current_level];
}

void Game::OnKeyActionCallback(Window* window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            m_key_states[key] = true;
        else if (action == GLFW_RELEASE)
            m_key_states[key] = false;
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

    const glm::vec2 player_size = glm::vec2(100.f, 20.f);
    const glm::vec2 player_pos = getPaddleRestPosition(player_size);
    Texture2D* paddle_sprite = ResourceManager::LoadTexture("paddle.png", true, "paddle");

    m_player = std::make_unique<Player>(player_pos, glm::vec2(0.f), player_size, paddle_sprite);
    m_player->m_max_pos_x = m_window->m_width;
    m_player->RenewLifePoints(3);

    const float ball_radius = 16.f;
    const glm::vec2 ball_pos = glm::vec2(player_pos.x + player_size.x * .5f - ball_radius, player_pos.y - player_size.y * 2.f);
    const glm::vec2 ball_velocity = BALL_INITIAL_VELOCITY;
    Texture2D* ball_sprite = ResourceManager::LoadTexture("awesomeface.png", true, "ball");

    m_ball = std::make_unique<BallObject>(ball_pos, ball_velocity, ball_radius, ball_sprite);
}


void Game::processInput(float dt)
{
    switch(m_state)
    {
        case GAME_ACTIVE:
            m_player->ProcessInput(dt, m_key_states);
            if (m_key_states[GLFW_KEY_SPACE]){
                m_ball->m_stuck = false;
            }

            if (m_key_states[GLFW_KEY_PAGE_DOWN]){
                GetCurrentLevel().DestroyOneBlock();
            }

            break;
    }
}

void Game::processCollisions()
{
    for (GameObject& brick: GetCurrentLevel().GetBricks())
    {
        if (brick.m_is_destroyed) continue;
        CollisionData brick_collision = m_ball->CheckCollision(brick);
        if (!brick_collision.m_collided) continue;

        if (!brick.m_is_solid)
        {
            brick.m_is_destroyed = true;
        }

        m_ball->OnCollision(brick_collision);
    }

    CollisionData paddle_collision = m_ball->CheckCollision(*m_player);
    if (paddle_collision.m_collided){
        m_ball->OnPaddleCollision(*m_player, paddle_collision);
    }
}

void Game::update(float dt)
{
    if (m_ball->m_stuck){
        m_ball->m_position = m_player->GetBallSlotPosition(*m_ball);
    }
    else{
        m_ball->Move(dt, m_window->m_width);
    }

    if (GetCurrentLevel().IsCompleted())
    {
        loadNextLevel();
        return;
    }

    if (isBallOutOfScreen()) // Lose a life or Game over
    {
        if (m_player->HasLifePoints()){
            m_player->LoseLifePoint();
            m_ball->Reset(m_player->GetBallSlotPosition(*m_ball), BALL_INITIAL_VELOCITY);
        }
        else{
            m_player->RenewLifePoints(3);
            GetCurrentLevel().Reset();
            m_player->m_position.x = m_window->m_width * .5f - m_player->m_size.x * .5f;
        }
    }
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
            current_level.Draw(*renderer);
            m_player->Draw(*renderer);
            m_ball->Draw(*renderer);
            break;
    }
}

bool Game::loadNextLevel()
{
    if (m_current_level + 1 == m_levels.size()) 
        return false;
    m_current_level++;
    return true;
}


glm::vec2 Game::getPaddleRestPosition(const glm::vec2& paddle_size) const
{
    return glm::vec2(
        m_window->m_width*.5f - paddle_size.x*0.5f,
        m_window->m_height - paddle_size.y
    );
}

bool Game::isBallOutOfScreen() const
{
    const float margin_y = m_ball->m_size.y * 6.f;
    return m_ball->m_position.y + m_ball->m_size.y > m_window->m_height + margin_y;
}
