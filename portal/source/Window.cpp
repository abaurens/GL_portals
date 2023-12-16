#include "App.hpp"

#include "Window.hpp"

#include <glad/gl.h>
#include <glm/ext.hpp> // glm::perspective

// event propagation

Window::Window() noexcept : m_cursorSave(0, 0), m_handle(nullptr) {}

Window::Window(Window &&mv) noexcept : m_cursorSave(mv.m_cursorSave), m_handle(mv.m_handle)
{
  mv.m_cursorSave = { 0, 0 };
  mv.m_handle = nullptr;
}

Window::~Window() noexcept
{
  clear();
}

Window::operator GLFWwindow *() const
{
  return m_handle;
}


bool Window::create(const std::string_view title, int width, int height)
{
  //glfwWindowHint(GLFW_DOUBLEBUFFER, true);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  m_handle = glfwCreateWindow(1280, 720, title.data(), nullptr, nullptr);
  if (!m_handle)
    return false;

  initEventCallbacks();

  if (glfwRawMouseMotionSupported())
    glfwSetInputMode(m_handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  glfwMakeContextCurrent(m_handle);

  // only load opengl if not already loaded
  if (glGetString == nullptr)
    gladLoadGL(glfwGetProcAddress);

  onResize(width, height);
  return true;
}

void Window::update()
{
  glfwPollEvents();
}

void Window::render()
{
  glfwSwapBuffers(m_handle);
}

void Window::clear()
{
  if (m_handle)
    glfwDestroyWindow(m_handle);

  m_cursorSave = { 0, 0 };
  m_handle = nullptr;
}

void Window::close()
{
  glfwSetWindowShouldClose(m_handle, GLFW_TRUE);
}

bool Window::isOpen() const
{
  return !glfwWindowShouldClose(m_handle);
}

void Window::setVSync(bool enabled)
{
  glfwSwapInterval(enabled ? 1 : 0);
}


void Window::onResize(int width, int height)
{
  float ratio;
  ratio = width / (float)height;

  glViewport(0, 0, width, height);
  App::proj = glm::perspective(70.0f, ratio, 0.1f, 50.0f);
}

void Window::onClick(int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS && App::paused)
  {
    App::paused = false;
    glfwGetCursorPos(m_handle, &m_cursorSave.x, &m_cursorSave.y);
    glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(m_handle, 0, 0);
  }
}

void Window::onKeyboard(int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    if (!App::paused)
    {
      App::paused = true;
      glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetCursorPos(m_handle, m_cursorSave.x, m_cursorSave.y);
    }
    else
      close();
  }

  if (action != GLFW_PRESS)
    return;

  //if (key == GLFW_KEY_P)
  //{
  //  std::cout << "Camera:\n"
  //    << "  x: " << camera.position.x << "\n"
  //    << "  y: " << camera.position.y << "\n"
  //    << "  z: " << camera.position.z << "\n"
  //    << "  yaw: " << camera.yaw << "\n"
  //    << "  pitch: " << camera.pitch << std::endl;
  //}
}




void Window::initEventCallbacks()
{
  glfwSetWindowUserPointer(m_handle, this);

  glfwSetFramebufferSizeCallback(m_handle, [](GLFWwindow *ptr, int width, int height) {
    Window *window = (Window *)glfwGetWindowUserPointer(ptr);
    return window->onResize(width, height);
  });

  glfwSetMouseButtonCallback(m_handle, [](GLFWwindow *ptr, int button, int action, int mods)
  {
    Window *window = (Window *)glfwGetWindowUserPointer(ptr);
    return window->onClick(button, action, mods);
  });

  glfwSetKeyCallback(m_handle, [](GLFWwindow *ptr, int key, int scancode, int action, int mods) {
    Window *window = (Window *)glfwGetWindowUserPointer(ptr);
    return window->onKeyboard(key, scancode, action, mods);
  });
}
