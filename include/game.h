#pragma once


enum GameState { GAME_ACTIVE, GAME_MENU, GAME_WIN };

class Game {
public:
  Game(unsigned int width, unsigned int height);
  ~Game();

  void Run();

  void OnKeyActionCallback(class Window* window, int key, int scancode, int action, int mode);
  void OnWindowResized(Window* window, int width, int height);

private:
  void ProcessInput(float dt);
  void Update(float dt);
  void Render();

public:
  bool m_keys[1024];
  GameState m_state;
  unsigned int m_width, m_height;
};
