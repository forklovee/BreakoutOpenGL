cmake_minimum_required(VERSION 3.10)
project(BreakoutOpenGL)


# find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_library(glad
    libs/glad/src/glad.c)
target_include_directories(glad PUBLIC
    libs/glad/include)

add_executable(Breakout
    src/main.cpp)

target_include_directories(Breakout PRIVATE
    include
)

target_sources(Breakout PRIVATE
    src/game.cpp
    src/window.cpp
    src/shader.cpp
    src/texture.cpp
    src/resource_manager.cpp
    src/sprite_renderer.cpp
    src/gameobject.cpp
    src/gamelevel.cpp
    src/player.cpp
)

target_link_libraries(Breakout glad glfw)