#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Game;

class Window
{
public:
    Window(Game* game, const char* name, unsigned int width, unsigned int height);
    ~Window();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

    void Refresh();

    bool ShouldClose() const;

public:
    GLFWwindow* m_window_ptr;
    Game* m_game;
    std::string m_name;
    unsigned int m_width, m_height;
};