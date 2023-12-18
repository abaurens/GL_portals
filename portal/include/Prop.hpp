#pragma once

#include "Texture.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>


#include <vector>

struct Prop
{
  glm::vec3 scale = { 1, 1, 1 };
  glm::vec3 pos = { 0, 0, 0 };
  glm::quat rot = glm::quat({ 0, 0, 0 });

  Material *material = nullptr;
  Mesh *mesh = nullptr;

  void render(const Camera &camera) const;

private:
  const Shader &getShader() const;
};