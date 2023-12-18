#pragma once

#include "Shader.hpp"
#include "Texture.hpp"

struct Material
{
  Shader *m_shader;
  std::vector<Texture*> m_textures;
};
