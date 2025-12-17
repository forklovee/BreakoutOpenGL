#include "post_processor.h"
#include "texture.h"

#include <GL/gl.h>
#include <cstdint>
#include <iostream>
#include <shader.h>

PostProcessor::PostProcessor(Shader* shader, uint32_t width, uint32_t height)
    : m_post_processing_shader(shader), m_screen_texture(), m_width(width), m_height(height)
{
    glGenFramebuffers(1, &m_MSFBO);
    glGenFramebuffers(1, &m_FBO);
    glGenRenderbuffers(1, &m_RBO);

    glBindFramebuffer(GL_FRAMEBUFFER, m_MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO\n";
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    m_screen_texture.Generate(m_width, m_height, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_screen_texture.m_id, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::POSTPROCESSOR: Failed to initialize FBO\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    initRenderData();
    m_post_processing_shader->SetInteger("scene", 0, true);
    float offset = 1.f/300.f;
    float offsets[9][2] = {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right    
    };
    glUniform2fv(glGetUniformLocation(m_post_processing_shader->m_id, "offsets"), 9, (float*)offsets);
    int edge_kernel[9] = {
        -1, -1, -1,
        -1, 8, -1,
        -1, -1, -1,
    };
    glUniform1iv(glGetUniformLocation(m_post_processing_shader->m_id, "edge_kernel"), 9, edge_kernel);
    float blur_kernel[9] = {
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };
    glUniform1fv(glGetUniformLocation(m_post_processing_shader->m_id, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::Begin()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_MSFBO);
    glClearColor(0.f, 1.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::End()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glViewport(0, 0, m_width, m_height);
}

void PostProcessor::Update(float dt)
{
    if (m_chaos_time > 0.f){
        m_chaos_time -= dt;
        if (m_chaos_time <= 0.f){
            DisableEffect(Effect::CHAOS);
        }
    }
    if (m_confuse_time > 0.f){
        m_confuse_time -= dt;
        if (m_confuse_time <= 0.f){
            DisableEffect(Effect::CONFUSE);
        }
    }
    if (m_shake_time > 0.f){
        m_shake_time -= dt;
        if (m_shake_time <= 0.f){
            DisableEffect(Effect::SHAKE);
        }
    }
}

void PostProcessor::Render(float time)
{
    m_post_processing_shader->Use();
    m_post_processing_shader->SetFloat("time", time);
    m_post_processing_shader->SetInteger("confuse", m_confuse);
    m_post_processing_shader->SetInteger("chaos", m_chaos);
    m_post_processing_shader->SetInteger("shake", m_shake);

    glActiveTexture(GL_TEXTURE0);
    m_screen_texture.Bind();

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::Resize(uint32_t window_width, uint32_t window_height)
{
    m_width = window_width;
    m_height = window_height;

    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, m_width, m_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    m_screen_texture.Generate(m_width, m_height, nullptr);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_screen_texture.m_id, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::POSTPROCESSOR: FBO not complete after Resize\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void PostProcessor::EnableEffect(Effect effect, float duration)
{
    switch (effect) {
        case Effect::CHAOS:
            m_chaos = true;
            m_chaos_time = duration;
            break;
        case Effect::CONFUSE:
            m_confuse = true;
            m_confuse_time = duration;
            break;
        case Effect::SHAKE:
            m_shake = true;
            m_shake_time = duration;
            break;
    }
}

void PostProcessor::DisableEffect(Effect effect)
{
    switch (effect) {
        case Effect::CHAOS:
            m_chaos = false;
            m_chaos_time = -1.f;
            break;
        case Effect::CONFUSE:
            m_confuse = false;
            m_confuse_time = -1.f;
            break;
        case Effect::SHAKE:
            m_shake = false;
            m_shake_time = -1.f;
            break;
    }
}


void PostProcessor::initRenderData()
{
    uint32_t VBO;
    float vertices[] = {
        // pos        // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
