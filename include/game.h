#pragma once

#include "gamelevel.h"
#include "player.h"
#include "window.h"
#include <memory>

enum GameState { GAME_ACTIVE, GAME_MENU, GAME_WIN };

class Game {
public:
  Game() = default;
  ~Game();

  void Run(unsigned int screen_width, unsigned int screen_height);

  void OnKeyActionCallback(class Window* window, int key, int scancode, int action, int mode);
  void OnWindowResized(Window* window, int width, int height);

private:
  void init();

  void processInput(float dt);
  void update(float dt);
  void render();

public:
  bool m_keys[1024];
  GameState m_state;
  std::unique_ptr<Window> m_window;

  unsigned int m_current_level;
  std::vector<GameLevel> m_levels;

  std::unique_ptr<Player> m_player;
};
