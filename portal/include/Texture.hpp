#pragma once

#include "Image.hpp"

#include <glad/gl.h>

class Texture
{
public:
  Texture() noexcept : m_id(0) {};
  Texture(const Image &img) noexcept : Texture() { upload(img); }

  void upload(const Image &image)
  {
    if (!glIsTexture(m_id))
      glGenTextures(1, &m_id);

    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  void clear()
  {
    if (glIsTexture(m_id))
      glDeleteTextures(1, &m_id);
    m_id = 0;
  }

  operator GLuint() const { return m_id; }

private:
  GLuint m_id;
};