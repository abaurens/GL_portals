#include "Prop.hpp"

#include <iostream>

void Prop::render(const Camera &camera) const
{
  // nothing to render
  if (!mesh || !*mesh)
    return;

  const Shader &shader = getShader();
  // material properties
  glUseProgram(shader.id());

  if (material)
  {
    for (size_t i = 0; i < material->m_textures.size(); ++i)
    {
      glActiveTexture(GL_TEXTURE0 + (GLenum)i);
      glBindTexture(GL_TEXTURE_2D, *material->m_textures[i]);
    }
  }

  // upload MVP matrix
  float time = (float)glfwGetTime();
  glm::mat4 trns = glm::mat4(1);

  trns = trns * glm::translate(glm::mat4(1), pos);
  trns = trns * glm::mat4_cast(rot);
  trns = trns * glm::scale(glm::mat4(1), scale);

  const glm::mat4 mvp = camera.getProj() * camera.getView() * trns;
  glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, (const GLfloat *)&mvp);

  mesh->render(camera, shader);
}

const Shader &Prop::getShader() const
{
  if (!material || !material->m_shader)
    return Shader::getDefaultShader();
  return *material->m_shader;
}
