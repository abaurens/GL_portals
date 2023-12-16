#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <string_view>

class Window
{
public:
  Window() noexcept;
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

protected:
  void onResize(int width, int height);
  void onClick(int button, int action, int mods);
  void onKeyboard(int key, int scancode, int action, int mods);

private:
  void initEventCallbacks();

  glm::dvec2 m_cursorSave;
  GLFWwindow *m_handle = nullptr;
};