#pragma once

#include "portal.hpp"

#include "Scene.hpp"
#include "Window.hpp"
#include "UserInterface.hpp"

#include <vector>

class App
{
public:
  App();

  void run();

protected:
  void init();
  void loop();
  void stop();

  void update(float timestep);
  void render();

public:
  void onResize(Window &window, int width, int height);
  void onClick(Window &window, int button, int action, int mods);
  void onKeyboard(Window &window, int key, int scancode, int action, int mods);

  Scene &getCurrentScene() { return scene; };

private:
  bool running = false;

  UserInterface ui;
  Camera camera;
  Window window;

  Scene scene;

private:
  glm::dvec2 m_cursorSave;
};