#pragma once

#include "portal.hpp"

#include "Mesh.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Window.hpp"

#include <vector>

class App
{
public:

  void run();

protected:
  void onInit();
  void onLoop();
  void onStop();

  void update(float timestep);
  void render();

public:
  static inline bool paused = true;
  static inline glm::mat4 proj = glm::mat4(1);

private:


  Shader flat;

  Shader flat_tex;
  Texture texture;

  Material flat_tex_mat;
  Material flat_white_mat;

  // globally accessible
  Camera camera;
  Window window;
  std::vector<Mesh> meshes;
};