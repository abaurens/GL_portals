#pragma once

#include "portal.hpp"
#include "GLFW/glfw3.h"

class App;

class UserInterface
{
public:
  UserInterface(App &app);

  void init(GLFWwindow *window);
  void clear();

  void update();
  void render();

private:
  App &m_app;
};