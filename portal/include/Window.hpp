#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <string_view>

class App;

class Window
{
public:
  Window(App &app) noexcept;
  Window(Window &&mv) noexcept;
  ~Window() noexcept;

  operator GLFWwindow*() const;

  bool create(const std::string_view title, int width, int height);
  void update();
  void render();
  void clear();
  void close();
  void setVSync(bool enabled);

  bool isOpen() const;

private:
  void initEventCallbacks();

  App &m_app;
  GLFWwindow *m_handle = nullptr;
};