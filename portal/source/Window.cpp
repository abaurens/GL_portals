#include "App.hpp"

#include "Window.hpp"

#include <glad/gl.h>
#include <glm/ext.hpp> // glm::perspective

// event propagation

Window::Window(App &app) noexcept : m_app(app), m_handle(nullptr) {}

Window::Window(Window &&mv) noexcept : m_app(mv.m_app), m_handle(mv.m_handle)
{
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

  m_app.onResize(*this, width, height);
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



void Window::initEventCallbacks()
{
  glfwSetWindowUserPointer(m_handle, this);

  glfwSetFramebufferSizeCallback(m_handle, [](GLFWwindow *ptr, int width, int height) {
    Window *window = (Window *)glfwGetWindowUserPointer(ptr);
    return window->m_app.onResize(*window, width, height);
  });

  glfwSetMouseButtonCallback(m_handle, [](GLFWwindow *ptr, int button, int action, int mods)
  {
    Window *window = (Window *)glfwGetWindowUserPointer(ptr);
    return window->m_app.onClick(*window, button, action, mods);
  });

  glfwSetKeyCallback(m_handle, [](GLFWwindow *ptr, int key, int scancode, int action, int mods) {
    Window *window = (Window *)glfwGetWindowUserPointer(ptr);
    return window->m_app.onKeyboard(*window, key, scancode, action, mods);
  });
}
