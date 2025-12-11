#include "window.h"
#include "game.h"

#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(Game* game, const char* name, unsigned int width, unsigned int height)
    :m_game(game), m_name(name), m_width(width), m_height(height)
{
    std::cout << "Creating a window '"<<name<<"' ("<<width<<","<<height<<")\n";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif


    m_window_ptr = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_window_ptr);
    glfwSetWindowUserPointer(m_window_ptr, this);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glfwSetKeyCallback(m_window_ptr, Window::keyCallback);
    glfwSetFramebufferSizeCallback(m_window_ptr, Window::framebufferSizeCallback);

    glViewport(0, 0, m_width, m_height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Window::~Window()
{
    glfwDestroyWindow(m_window_ptr);
    glfwTerminate();
}

void Window::Clear(){
    glClearColor(0.1f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Present()
{
    glfwSwapBuffers(m_window_ptr);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_window_ptr);
}


void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(self->m_window_ptr, true);
    }

    self->m_game->OnKeyActionCallback(self, key, scancode, action, mode);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    glViewport(0, 0, width, height);
    self->m_width = width;
    self->m_height = width;

    self->m_game->OnWindowResized(self, width, height);
}