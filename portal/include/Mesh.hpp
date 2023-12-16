#pragma once


#include "Camera.hpp"
#include "Material.hpp"


#include <glad/gl.h>
#include <glm/glm.hpp>

#include <vector>

struct Mesh
{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<GLushort>  indicies;

  glm::mat4 transform = glm::mat4(1);

  Mesh() noexcept;
  Mesh(Mesh &&mv) noexcept;
  Mesh(const Mesh &mv);
  ~Mesh();

  constexpr Mesh &operator=(const Mesh &other)
  {
    uvs = other.uvs;
    vertices = other.vertices;
    indicies = other.indicies;

    transform = other.transform;


    m_is_messy = true;
    m_material = other.m_material;

    if (!other.m_is_messy && glIsVertexArray(other.m_vertex_array))
      upload();

    return *this;
  }

  Mesh &operator=(Mesh &&other) noexcept
  {
    uvs = std::move(other.uvs);
    vertices = std::move(other.vertices);
    indicies = std::move(other.indicies);

    transform = other.transform;

    m_uv_buffer = other.m_uv_buffer;
    m_vertex_buffer = other.m_vertex_buffer;
    m_vertex_array = other.m_vertex_array;

    m_is_messy = other.m_is_messy;
    m_material = other.m_material;

    other.reset();
    return *this;
  }

  void upload();
  void clear();

  void render(const Camera &camera) const;

  void setMaterial(const Material *material);

private:
  void reset();

  const Shader &getShader() const;

  void disableVertAttribs() const;
  void enableVertAttribs() const;

  bool   m_is_messy = true;
  const Material *m_material = nullptr;

  GLuint m_uv_buffer;
  GLuint m_vertex_buffer;
  GLuint m_vertex_array;
};
