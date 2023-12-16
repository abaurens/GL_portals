#include "App.hpp"

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;
namespace chrono = std::chrono;

void App::run()
{
  onInit();
  onLoop();
  onStop();
}

// callbacks
static void error_callback(int error, const char *description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void App::onInit()
{
  glfwInit();
  glfwSetErrorCallback(error_callback);

  window.create(PROJECT_NAME, 1280, 720);
  window.setVSync(true);


  // load shaders
  flat.addFile(Shader::Type::vertex, "./shaders/flat.vert");
  flat.addFile(Shader::Type::fragment, "./shaders/flat.frag");
  flat.compile();

  flat_tex.addFile(Shader::Type::vertex, "./shaders/flat_texture.vert");
  flat_tex.addFile(Shader::Type::fragment, "./shaders/flat_texture.frag");
  flat_tex.compile();


  // load textures
  Image img;
  img.load("./assets/test.png", 3);
  texture.upload(img);


  // generate materials
  flat_tex_mat.m_shader = flat_tex;
  flat_tex_mat.m_textures.push_back(texture);

  flat_white_mat.m_shader = flat;

  // generate meshes
  meshes.push_back(create_portal_mesh());
  meshes.emplace_back(create_portal_mesh());

  meshes[0].setMaterial(&flat_tex_mat);

  // 90° angle + slightly higher
  meshes[0].transform = glm::translate(glm::mat4(1), glm::vec3(0, -2, -1));
  meshes[1].transform = glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 0, 1)) * glm::translate(glm::mat4(1), glm::vec3(0, -2, -1.2));


  // set camera position so they both appear on start
  camera = { { 3.2, 2, 1.0 }, {{ 4.1, 0.5 }} };

  glEnable(GL_CULL_FACE);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void App::onLoop()
{
  chrono::steady_clock::time_point now;
  chrono::steady_clock::time_point last = chrono::steady_clock::now();

  // rendering loop
  while (window.isOpen())
  {
    now = chrono::steady_clock::now();
    const chrono::milliseconds millis = chrono::duration_cast<chrono::milliseconds>(now - last);
    last = now;

    //std::cout << "timestep : " << millis << std::endl;
    window.update();
    update((float)millis.count() / 1000.0f);
    render();
    window.render();
  }
}

void App::onStop()
{
  texture.clear();

  flat.clear();
  flat_tex.clear();

  for (Mesh &mesh : meshes)
    mesh.clear();
  meshes.clear();

  window.clear();
  glfwTerminate();
}


void App::update(float timestep)
{
  if (!paused)
    camera.update(timestep, window);
}

void App::render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const Mesh &mesh : meshes)
  {
    mesh.render(camera);
  }
}

