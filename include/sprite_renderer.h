#pragma once

#include "shader.h"
#include "texture.h"

class SpriteRenderer
{
public:
    SpriteRenderer(Shader* shader);
    ~SpriteRenderer();

    void DrawSprite(Texture2D* texture, glm::vec2 position, 
        glm::vec2 size = glm::vec2(16.f, 16.f), float rotation_deg = 0.f,
        glm::vec3 color = glm::vec3(1.f));
    
private:
    void initRenderData();

private:
    Shader* m_shader;
    unsigned int m_quadVAO;
};