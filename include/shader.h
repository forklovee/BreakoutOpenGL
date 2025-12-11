#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    Shader() { }
    // copying is forbidden!
    Shader(const Shader&) = delete;
    Shader& operator =(const Shader&) = delete;

    Shader(Shader&& other) noexcept : m_id(other.m_id)
    {
        other.m_id = 0;
    }

    ~Shader();

    Shader  &Use();

    void    Compile(const char *vertex_source, const char *fragment_source, const char *geometry_source = nullptr); // note: geometry source code is optional 
    void    SetFloat    (const char *name, float value, bool use_shader = false);
    void    SetInteger  (const char *name, int value, bool use_shader = false);
    void    SetVector2f (const char *name, float x, float y, bool use_shader = false);
    void    SetVector2f (const char *name, const glm::vec2 &value, bool use_shader = false);
    void    SetVector3f (const char *name, float x, float y, float z, bool use_shader = false);
    void    SetVector3f (const char *name, const glm::vec3 &value, bool use_shader = false);
    void    SetVector4f (const char *name, float x, float y, float z, float w, bool use_shader = false);
    void    SetVector4f (const char *name, const glm::vec4 &value, bool use_shader = false);
    void    SetMatrix4  (const char *name, const glm::mat4 &matrix, bool use_shader = false);
private:
    void    checkCompileErrors(unsigned int object, std::string type);

public:
    unsigned int m_id;
};