#include "game.h"

#include "ball.h"
#include "gamelevel.h"
#include "gameobject.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "powerup.h"
#include "resource_manager.h"
#include "shader.h"
#include "sprite_renderer.h"
#include "texture.h"
#include "window.h"

#include <algorithm>
#include <glm/common.hpp>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <string>
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

    m_postprocessor->Resize(width, height);
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
    ResourceManager::LoadTexture("trzeciaszyna.jpg", false, "background");

    Shader* post_processor_shader = ResourceManager::LoadShader("post_processor.vs", "post_processor.frag", nullptr, "post_processor");
    m_postprocessor = std::make_unique<PostProcessor>(post_processor_shader, m_window->m_width, m_window->m_height);

    Shader* particle_shader = ResourceManager::LoadShader("particle.vs", "particle.frag", nullptr, "square_particle");
    particle_shader->Use();
    particle_shader->SetInteger("image", 0);
    particle_shader->SetMatrix4("projection", projection);

    Texture2D* particle_texture = ResourceManager::LoadTexture("particle.png", true, "square");
    m_particles = std::make_unique<ParticleGenerator>(particle_shader, particle_texture, 500);

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
    // Ball-Brick collision detection
    for (GameObject& brick: GetCurrentLevel().GetBricks())
    {
        if (brick.m_is_destroyed) continue;
        CollisionData brick_collision = m_ball->CheckCollision(brick);
        if (!brick_collision.m_collided) continue;

        if (!brick.m_is_solid)
        {
            brick.m_is_destroyed = true;
            spawnPowerUp(brick);
        }
        else
        {
            m_postprocessor->EnableEffect(PostProcessor::Effect::SHAKE, 0.05f);
        }

        m_ball->OnCollision(brick_collision);
    }

    // Ball-Paddle collision detection
    CollisionData paddle_collision = m_ball->CheckCollision(*m_player);
    if (paddle_collision.m_collided){
        if (!m_ball->m_stuck){
            m_ball->ResetVelocity();
            m_ball->m_stuck = m_ball->m_sticky;
        }
        m_ball->OnPaddleCollision(*m_player, paddle_collision);
    }

    // Paddle-Powerup collision detection
    for (PowerUp& power_up: m_spawned_powerups)
    {
        if (power_up.m_is_destroyed) continue;

        CollisionData paddle_collision = m_player->CheckCollision(power_up);
        if (!paddle_collision.m_collided) continue;
        
        // Check if active power up already exists
        if (PowerUp* active_power_up = getActivePowerUpByType(power_up.m_type)){
            power_up.m_is_destroyed = true;
            active_power_up->ResetDuration();
            continue;
        }

        PowerUpContext context{*m_player, *m_ball, *m_postprocessor};
        power_up.Activate(context);
    }
}

void Game::update(float dt)
{
    m_particles->Update(dt, *m_ball, 2, glm::vec2(m_ball->m_radius * 0.5f));
    m_postprocessor->Update(dt);

    updatePowerUps(dt);

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
    m_postprocessor->Begin();

    Texture2D* background = ResourceManager::GetTexture("background");
    renderer->DrawSprite(background, glm::vec2(0.0f, 0.0f), glm::vec2(720.0f, 720.0f));

    switch(m_state)
    {
        case GAME_ACTIVE:{
            GameLevel& current_level = m_levels.at(m_current_level);
            current_level.Draw(*renderer);
            m_particles->Draw();
            m_player->Draw(*renderer);
            m_ball->Draw(*renderer);

            for (PowerUp& power_up: m_spawned_powerups){
                power_up.Draw(*renderer);
            }

            break;
        }
        case GAME_WIN:{
            break;
        }
    }

    m_postprocessor->End();
    m_postprocessor->Render(glfwGetTime());
}

void Game::updatePowerUps(float dt)
{
    PowerUpContext context{*m_player, *m_ball, *m_postprocessor};

    for (PowerUp& power_up: m_spawned_powerups){
        power_up.Move(dt);
        if (!power_up.m_activated && power_up.m_position.y >= m_window->m_height){
            power_up.m_is_destroyed = true;
            continue;
        }

        if (!power_up.m_activated) continue;

        if (power_up.m_duration > 0.f){
            power_up.m_duration -= dt;
            continue;
        }
        power_up.Deactivate(context);
    }

    auto remove_it = std::remove_if(m_spawned_powerups.begin(), m_spawned_powerups.end(),
            [](const PowerUp& power_up) { return power_up.m_is_destroyed && !power_up.m_activated; });
    m_spawned_powerups.erase(remove_it,m_spawned_powerups.end());
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

bool Game::canSpawnPowerup(uint16_t chance)
{
    return rand() % chance == 0;
}

void Game::spawnPowerUp(GameObject& object)
{
    const std::array<PowerUp::Type, 6> power_up_types{
        PowerUp::Type::SPEED,
        PowerUp::Type::STICKY,
        PowerUp::Type::PASS_THROUGH,
        PowerUp::Type::PADDLE_SIZE_INCREASE,
        PowerUp::Type::CONFUSE,
        PowerUp::Type::CHAOS
    };

    for (const PowerUp::Type& power_up_type: power_up_types)
    {
        if (!canSpawnPowerup(75)) continue;
        PowerUp power_up = PowerUp(power_up_type, object.m_position);
        m_spawned_powerups.emplace_back(std::move(power_up));
        std::cout << "Power up " << power_up.GetName() << " spawned!\n";
        break;
    }
}

PowerUp* Game::getActivePowerUpByType(PowerUp::Type type)
{
    auto it = std::find_if(m_spawned_powerups.begin(), m_spawned_powerups.end(),
        [type](const PowerUp& power_up) { 
            return power_up.m_type == type && power_up.m_activated;
        });
    if (it == m_spawned_powerups.end()) return nullptr;
    return &(*it);
}