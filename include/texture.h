#pragma once

#include <glad/glad.h>

class Texture2D
{
public:
    Texture2D();
    // copying is forbidden!
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator =(const Texture2D&) = delete;

    Texture2D(Texture2D&& other) : m_id(other.m_id),
        m_width(other.m_width), m_height(other.m_height),
        m_internal_format(other.m_internal_format), m_image_format(other.m_image_format),
        m_wrap_s(other.m_wrap_s), m_wrap_t(other.m_wrap_t),
        m_filter_min(other.m_filter_min), m_filter_max(other.m_filter_max)
    {
        other.m_id = 0;
    }

    ~Texture2D();

    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    void Bind() const;

public:
    unsigned int m_id;
    unsigned int m_width, m_height;

    unsigned int m_internal_format;
    unsigned int m_image_format;

    unsigned int m_wrap_s; // wrapping mode on S axis
    unsigned int m_wrap_t; // wrapping mode on T axis
    unsigned int m_filter_min; // filtering mode if texture pixels < screen pixels
    unsigned int m_filter_max; // filtering mode if texture pixels > screen pixels

};