#pragma once

#include "texture.h"
#include <cstdint>

class Shader;

class PostProcessor
{
public:
    enum class Effect
    {
        CONFUSE,
        CHAOS,
        SHAKE
    };

    PostProcessor(Shader* shader, uint32_t width, uint32_t height);

    void Begin();
    void End();

    void Update(float dt);
    void Render(float time);
    void Resize(uint32_t window_width, uint32_t window_height);

    void EnableEffect(Effect effect, float duration = -1.f);
    void DisableEffect(Effect effect);

private:
    void initRenderData();

public:
    Shader* m_post_processing_shader;
    Texture2D m_screen_texture{};
    uint32_t m_width{}, m_height{};

    bool m_confuse{}, m_chaos{}, m_shake{};
    float m_confuse_time{}, m_chaos_time{}, m_shake_time{};

private:
    uint32_t m_MSFBO{}, m_FBO{};
    uint32_t m_RBO{};
    uint32_t m_VAO{};

};