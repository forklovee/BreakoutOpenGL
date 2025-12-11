#include "resource_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>

std::map<std::string, Texture2D> ResourceManager::m_textures;
std::map<std::string, Shader>    ResourceManager::m_shaders;

Shader* ResourceManager::LoadShader(const char *vert_shader_file, const char *frag_shader_file, const char *geo_shader_file, std::string name)
{
    const auto it = m_shaders.emplace(name, loadShaderFromFile(vert_shader_file, frag_shader_file, geo_shader_file)).first;
    return &it->second;
}

Shader* ResourceManager::GetShader(std::string name)
{
    const auto it = m_shaders.find(name);
    if (it == m_shaders.end())
        return nullptr;
    return &it->second;
}


Texture2D* ResourceManager::LoadTexture(const char *file, bool alpha, std::string name)
{
    const auto it = m_textures.emplace(name, loadTextureFromFile(file, alpha)).first;
    return &it->second;
}

Texture2D* ResourceManager::GetTexture(std::string name)
{
    const auto it = m_textures.find(name);
    if (it == m_textures.end())
        return nullptr;
    return &it->second;
}


void ResourceManager::Clear()
{
    m_shaders.clear();
    m_textures.clear();
}


Shader ResourceManager::loadShaderFromFile(const char *vert_shader_file, const char *frag_shader_file, const char *geo_shader_file)
{
    std::string vertex_code, fragment_code, geometry_code;
    try
    {
        std::ifstream vertex_file(vert_shader_file), fragment_file(frag_shader_file);
        std::stringstream vert_shader_stream, frag_shader_stream;
        vert_shader_stream << vertex_file.rdbuf();
        frag_shader_stream << fragment_file.rdbuf();
        vertex_file.close();
        fragment_file.close();
        vertex_code = vert_shader_stream.str();
        fragment_code = frag_shader_stream.str();

        if (geo_shader_file != nullptr)
        {
            std::ifstream geometry_file(geo_shader_file);
            std::stringstream geo_shader_stream;
            geo_shader_stream << geometry_file.rdbuf();
            geometry_file.close();
            geometry_code = geo_shader_stream.str();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR::SHADER: Failed to read shader files. " << e.what() << "\n";
    }

    Shader shader;
    shader.Compile(
        vertex_code.c_str(),
        fragment_code.c_str(),
        (geo_shader_file != nullptr) ? geometry_code.c_str() : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char *file, bool alpha)
{
    Texture2D texture;
    if (alpha)
    {
        texture.m_internal_format = GL_RGBA;
        texture.m_image_format = GL_RGBA;
    }
    int width, height, nr_channels;
    unsigned char* data = stbi_load(file, &width, &height, &nr_channels, 0);
    texture.Generate(width, height, data);
    stbi_image_free(data);
    return texture;
}