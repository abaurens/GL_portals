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

  Mesh() noexcept;
  Mesh(Mesh &&mv) noexcept;
  Mesh(const Mesh &mv);
  ~Mesh();

  constexpr Mesh &operator=(const Mesh &other)
  {
    uvs = other.uvs;
    vertices = other.vertices;
    indicies = other.indicies;

    m_is_messy = true;
    m_material = other.m_material;

    if (!other.m_is_messy && glIsVertexArray(other.m_vertex_array))
      upload();

    return *this;
  }


  glm::mat4 transform;

  void upload();
  void clear();

  void render(const Camera &camera) const;

  void setMaterial(const Material *material);

private:
  const Shader &getShader() const;

  void disableVertAttribs() const;
  void enableVertAttribs() const;

  bool   m_is_messy = true;
  const Material *m_material = nullptr;

  GLuint m_uv_buffer;
  GLuint m_vertex_buffer;
  GLuint m_vertex_array;
};
