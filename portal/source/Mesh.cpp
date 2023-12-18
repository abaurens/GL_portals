
#include "portal.hpp"

#include "Mesh.hpp"

#include "App.hpp"

Mesh::Mesh() noexcept : m_uv_buffer(0), m_vertex_buffer(0), m_vertex_array(0), m_is_messy(true)//, m_material(nullptr)
{}

Mesh::Mesh(Mesh &&mv) noexcept :
  uvs(mv.uvs), vertices(mv.vertices), indicies(mv.indicies),
  m_is_messy(mv.m_is_messy),
  m_uv_buffer(mv.m_uv_buffer),
  m_vertex_buffer(mv.m_vertex_buffer),
  m_vertex_array(mv.m_vertex_array)
{
  mv.reset();
}

Mesh::Mesh(const Mesh &other) :
  uvs(other.uvs),
  vertices(other.vertices),
  indicies(other.indicies),
  m_is_messy(true),
  m_uv_buffer(0),
  m_vertex_buffer(0),
  m_vertex_array(0)
{
  if (!other.m_is_messy && glIsVertexArray(other.m_vertex_array))
    upload();
}

Mesh::~Mesh()
{
  clear();
}

void Mesh::upload()
{
  if (!glIsVertexArray(m_vertex_array))
    glGenVertexArrays(1, &m_vertex_array);
  glBindVertexArray(m_vertex_array);

  if (!glIsBuffer(m_vertex_buffer))
    glGenBuffers(1, &m_vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glNamedBufferData(m_vertex_buffer, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

  if (!uvs.empty())
  {
    if (!glIsBuffer(m_uv_buffer))
      glGenBuffers(1, &m_uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer);
    glNamedBufferData(m_uv_buffer, vertices.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
  }

  m_is_messy = false;
}

void Mesh::clear()
{
  if (glIsBuffer(m_vertex_buffer))
    glDeleteBuffers(1, &m_vertex_buffer);

  if (glIsVertexArray(m_vertex_array))
    glDeleteVertexArrays(1, &m_vertex_array);

  reset();
}

void Mesh::render(const Camera &camera, const Shader &shader) const
{
  // bind VAO
  glBindVertexArray(m_vertex_array);

  // enable vertex shader attributes
  enableVertAttribs(shader);

  // draw the VAO
  glDrawElements(GL_TRIANGLES, (GLsizei)indicies.size(), GL_UNSIGNED_SHORT, &indicies[0]);

  // disable vertex shader attributes
  disableVertAttribs(shader);
}

void Mesh::reset()
{
  uvs.clear();
  vertices.clear();
  indicies.clear();

  m_is_messy = true;

  m_uv_buffer = 0;
  m_vertex_buffer = 0;
  m_vertex_array = 0;
}


void Mesh::disableVertAttribs(const Shader &shader) const
{
  const GLint vPos_loc = shader.getAttribute("vPos");

  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glDisableVertexArrayAttrib(m_vertex_array, vPos_loc);

  if (!uvs.empty())
  {
    const GLint vTex_loc = shader.getAttribute("vTex");

    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer);
    glDisableVertexArrayAttrib(m_vertex_array, vTex_loc);
  }
}

void Mesh::enableVertAttribs(const Shader &shader) const
{
  const GLint vPos_loc = shader.getAttribute("vPos");

  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glEnableVertexArrayAttrib(m_vertex_array, vPos_loc);
  glVertexAttribPointer(vPos_loc, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  if (!uvs.empty())
  {
    const GLint vTex_loc = shader.getAttribute("vTex");

    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer);
    glEnableVertexArrayAttrib(m_vertex_array, vTex_loc);
    glVertexAttribPointer(vTex_loc, 2, GL_FLOAT, GL_FALSE, (GLsizei)0, (void *)0);
  }
}
