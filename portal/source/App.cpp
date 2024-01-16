#include "App.hpp"

#include <chrono>
#include <iostream>

#include "imgui.h"

using namespace std::chrono_literals;
namespace chrono = std::chrono;

App::App() : window(*this), ui(*this) {}

void App::run()
{
  running = true;

  init();
  loop();
  stop();
}

// callbacks
static void error_callback(int error, const char *description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void App::init()
{
  glfwInit();
  glfwSetErrorCallback(error_callback);

  window.create(PROJECT_NAME, 1280, 720);
  window.setVSync(true);

  ui.init(window);

  scene.load("./assets/scene.json");

  // set camera position so they both appear on start
  camera.position = { 3.2, 2, 1.0 };
  camera.rotation = { 4.1, 0.5 };
  camera.setProjection(Camera::ProjType::perspective);

  glEnable(GL_CULL_FACE);
  //glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void App::loop()
{
  chrono::steady_clock::time_point now;
  chrono::steady_clock::time_point last = chrono::steady_clock::now();

  // rendering loop
  while (running && window.isOpen())
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

void App::stop()
{
  running = false;

  ui.clear();
  scene.cleanup();
  window.clear();
  glfwTerminate();
}


void App::update(float timestep)
{
  ui.update();

  const bool isMouseGrabbed = (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL);

  if (isMouseGrabbed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    camera.update(timestep, window);
}

void App::render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scene.render(camera);

  ui.render();
}




void App::onResize(Window &window, int width, int height)
{
  glViewport(0, 0, width, height);
  camera.setViewport(width, height);
}

void App::onClick(Window &window, int button, int action, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return;

  if (button == GLFW_MOUSE_BUTTON_2)
  {
    if (action == GLFW_PRESS)
    {
      glfwGetCursorPos(window, &m_cursorSave.x, &m_cursorSave.y);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      glfwSetCursorPos(window, 0, 0);
    }
    else if (action == GLFW_RELEASE)
    {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetCursorPos(window, m_cursorSave.x, m_cursorSave.y);
    }
  }

}

void App::onKeyboard(Window &window, int key, int scancode, int action, int mods)
{
  if (ImGui::GetIO().WantCaptureKeyboard)
    return;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    window.close();

  if (action != GLFW_PRESS)
    return;

  if (key == GLFW_KEY_P)
  {
    if (camera.getProjectionType() == Camera::ProjType::perspective)
      camera.setProjection(Camera::ProjType::orthographic);
    else
      camera.setProjection(Camera::ProjType::perspective);
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && key == GLFW_KEY_R)
  {
    std::cout << "Reloading scene..." << std::endl;
    scene.reload();
    std::cout << "Done !" << std::endl;
  }

  if (key == GLFW_KEY_O)
  {
    std::cout << "Camera:\n"
      << "  x: " << camera.position.x << "\n"
      << "  y: " << camera.position.y << "\n"
      << "  z: " << camera.position.z << "\n"
      << "  yaw: " << camera.yaw << "\n"
      << "  pitch: " << camera.pitch << std::endl;
  }
}