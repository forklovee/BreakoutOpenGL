#pragma once

#include "ball.h"
#include "gamelevel.h"
#include "gameobject.h"
#include "particle_generator.h"
#include "player.h"
#include "post_processor.h"
#include "powerup.h"
#include "window.h"
#include <memory>

enum GameState { GAME_ACTIVE, GAME_MENU, GAME_WIN };

class Game {
public:
  Game();
  ~Game();

  void Run(unsigned int screen_width, unsigned int screen_height);

  GameLevel& GetCurrentLevel();

  void OnKeyActionCallback(class Window* window, int key, int scancode, int action, int mode);
  void OnWindowResized(Window* window, int width, int height);

private:
  void init();

  void processInput(float dt);
  void processCollisions();
  void update(float dt);
  void updatePowerUps(float dt);
  void render();

  bool loadNextLevel();
  glm::vec2 getPaddleRestPosition(const glm::vec2& paddle_size) const;
  bool isBallOutOfScreen() const;

  bool canSpawnPowerup(uint16_t chance);
  void spawnPowerUp(GameObject& object);
  PowerUp* getActivePowerUpByType(PowerUp::Type type);

public:
  const glm::vec2 BALL_INITIAL_VELOCITY = glm::vec2(100.f, -350.f);

  GameState m_state;
  std::array<bool, 1024> m_key_states;

  std::unique_ptr<Window> m_window;
  std::unique_ptr<PostProcessor> m_postprocessor;
  std::unique_ptr<ParticleGenerator> m_particles;

  unsigned int m_current_level;
  std::vector<GameLevel> m_levels;

  std::unique_ptr<Player> m_player;
  std::unique_ptr<BallObject> m_ball;

  std::vector<PowerUp> m_spawned_powerups;

};
