#pragma once

#include "Window.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

#include <math.h>

struct Camera
{
public:
  enum class ProjType: bool
  {
    perspective,
    orthographic
  };

  glm::vec3 position = {0, 0, 0};
  union
  {
    glm::vec2 rotation = { 0, 0 };
    struct
    {
      float yaw;
      float pitch;
    };
  };

public:
  glm::vec3 up() const { return glm::vec3(0, 0, 1); }

  glm::vec3 right() const { return { cos(-yaw), sin(-yaw), 0 }; }

  glm::vec3 forward_2d() const { return { -sin(-yaw), cos(-yaw), 0 }; }

  glm::vec3 forward() const
  {
    // Forward vector from spherical coordinates
    // https://en.wikipedia.org/wiki/Spherical_coordinate_system

    const float cosp = cos(-pitch);
    return { cosp * -sin(-yaw), cosp * cos(-yaw), sin(-pitch)};
  }

  ProjType getProjectionType() const { return m_projType; }

  void setProjection(ProjType type)
  {
    if (type == m_projType)
      return;

    m_projType = type;
    rebuildProjectionMatrix();
  }

  void setViewport(int width, int height)
  {
    if (width == m_viewport.x && height == m_viewport.y)
      return;
    m_viewport = { width, height };
    rebuildProjectionMatrix();
  }

  glm::mat4 getView() const { return glm::lookAt(position, position + forward(), up()); }

  glm::mat4 getProj() const { return m_proj; }

  void update(float timestep, const Window &window)
  {
    constexpr float TOLLERANCE = 0.00001f;
    constexpr float HALF_PI = ((float)M_PI_2 - TOLLERANCE);
    constexpr float TWO_PI  = (float)M_PI * 2;

    float speed = 3.0f;
    float mouseSensivity = 0.3f;
    glm::vec3 movement = { 0, 0, 0 };

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
      speed *= 2.0f;

    if (glfwGetKey(window, GLFW_KEY_W))
      movement += forward_2d();
    if (glfwGetKey(window, GLFW_KEY_S))
      movement -= forward_2d();
    if (glfwGetKey(window, GLFW_KEY_D))
      movement += right();
    if (glfwGetKey(window, GLFW_KEY_A))
      movement -= right();
    if (glfwGetKey(window, GLFW_KEY_SPACE))
      movement += up();
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
      movement -= up();

    glm::dvec2 mouse_delta;
    glfwGetCursorPos(window, &mouse_delta.x, &mouse_delta.y);
    glfwSetCursorPos(window, 0, 0);

    position += (movement * speed * timestep);
    rotation += (glm::vec2(mouse_delta) * mouseSensivity * timestep);

    yaw = glm::mod(yaw, TWO_PI); // ensures no overgrowth 
    pitch = glm::clamp(pitch, -HALF_PI, HALF_PI); // ensures no screen inversion
  }

private:
  void rebuildProjectionMatrix()
  {
    float ratio = m_viewport.x / (float)m_viewport.y;
    if (m_projType == ProjType::perspective)
      m_proj = glm::perspective(70.0f, ratio, 0.1f, 50.0f);
    else
      m_proj = glm::ortho(0.0f, (float)m_viewport.x, 0.0f, (float)m_viewport.y);
  }

  ProjType m_projType = ProjType::perspective;
  glm::ivec2 m_viewport;
  glm::mat4  m_proj;
};