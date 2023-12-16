#pragma once

#include "portal.hpp"

#include "Mesh.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Material.hpp"

class Scene
{
public:
  void init();
  void cleanup();

  void render(const Camera &camera);

private:
  Shader flat;

  Shader flat_tex;
  Texture texture;
  Texture storageCubeTexture;

  Material flat_tex_mat;
  Material flat_white_mat;
  Material storageCube_mat;

  std::vector<Mesh> meshes;
};