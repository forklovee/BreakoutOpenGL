#pragma once

#include <map>
#include <string>

#include "texture.h"
#include "shader.h"

class ResourceManager
{
    public:
        static Shader* LoadShader(const char *vert_shader_file, const char *frag_shader_file, const char *geo_shader_file, std::string name);
        static Shader* GetShader(std::string name);

        static Texture2D* LoadTexture(const char *file, bool alpha, std::string name);
        static Texture2D* GetTexture(std::string name);

        static void Clear();
    private:
        ResourceManager() = default;

        static Shader loadShaderFromFile(const char *vert_shader_file, const char *frag_shader_file, const char *geo_shader_file = nullptr);
        static Texture2D loadTextureFromFile(const char *file, bool alpha);
    
    private:
        static std::map<std::string, Shader>    m_shaders;
        static std::map<std::string, Texture2D> m_textures;
};