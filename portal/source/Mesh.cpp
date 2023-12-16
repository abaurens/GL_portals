
#include "portal.hpp"

#include "Mesh.hpp"

#include "App.hpp"

Mesh::Mesh() noexcept : m_uv_buffer(0), m_vertex_buffer(0), m_vertex_array(0), m_is_messy(true), m_material(nullptr) {}

Mesh::Mesh(Mesh &&mv) noexcept :
  uvs(mv.uvs), vertices(mv.vertices), indicies(mv.indicies),
  m_uv_buffer(mv.m_uv_buffer),
  m_vertex_buffer(mv.m_vertex_buffer),
  m_vertex_array(mv.m_vertex_array),
  m_is_messy(mv.m_is_messy),
  m_material(mv.m_material)
{
  mv.uvs.clear();
  mv.vertices.clear();
  mv.indicies.clear();

  mv.m_uv_buffer = 0;
  mv.m_vertex_buffer = 0;
  mv.m_vertex_array = 0;
  mv.m_is_messy = true;
  mv.m_material = nullptr;
}

Mesh::Mesh(const Mesh &other):
  uvs(other.uvs),
  vertices(other.vertices),
  indicies(other.indicies),
  m_is_messy(true),
  m_material(other.m_material)
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
}

void Mesh::clear()
{
  if (glIsBuffer(m_vertex_buffer))
    glDeleteBuffers(1, &m_vertex_buffer);

  if (glIsVertexArray(m_vertex_array))
    glDeleteVertexArrays(1, &m_vertex_array);

  vertices.clear();
  indicies.clear();
  transform = glm::mat4(1);

  m_vertex_buffer = 0;
  m_vertex_array = 0;
}

void Mesh::render(const Camera &camera) const
{
  // material properties
  glUseProgram(getShader().id());

  if (m_material)
  {
    for (size_t i = 0; i < m_material->m_textures.size(); ++i)
    {
      glActiveTexture(GL_TEXTURE0 + (GLenum)i);
      glBindTexture(GL_TEXTURE_2D, m_material->m_textures[i]);
    }
  }
  //glBindTexture(GL_TEXTURE_2D, texture);

  // bind VAO
  glBindVertexArray(m_vertex_array);

  // enable vertex shader attributes
  enableVertAttribs();

  // upload MVP matrix
  const glm::mat4 mvp = App::proj * camera.getView() * transform;
  glUniformMatrix4fv(getShader().getUniform("MVP"), 1, GL_FALSE, (const GLfloat *)&mvp);

  // draw the VAO
  glDrawElements(GL_TRIANGLES, (GLsizei)indicies.size(), GL_UNSIGNED_SHORT, &indicies[0]);

  // disable vertex shader attributes
  disableVertAttribs();
}

void Mesh::setMaterial(const Material *material)
{
  m_material = material;
}


const Shader &Mesh::getShader() const
{
  if (!m_material)
    return Shader::getDefaultShader();
  return m_material->m_shader;
}

void Mesh::disableVertAttribs() const
{
  const GLint vPos_loc = getShader().getAttribute("vPos");

  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glDisableVertexArrayAttrib(m_vertex_array, vPos_loc);

  if (!uvs.empty())
  {
    const GLint vTex_loc = getShader().getAttribute("vTex");

    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer);
    glDisableVertexArrayAttrib(m_vertex_array, vTex_loc);
  }
}

void Mesh::enableVertAttribs() const
{
  const GLint vPos_loc = getShader().getAttribute("vPos");

  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glEnableVertexArrayAttrib(m_vertex_array, vPos_loc);
  glVertexAttribPointer(vPos_loc, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  if (!uvs.empty())
  {
    const GLint vTex_loc = getShader().getAttribute("vTex");

    glBindBuffer(GL_ARRAY_BUFFER, m_uv_buffer);
    glEnableVertexArrayAttrib(m_vertex_array, vTex_loc);
    glVertexAttribPointer(vTex_loc, 2, GL_FLOAT, GL_FALSE, (GLsizei)0, (void *)0);
  }
}
