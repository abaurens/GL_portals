#include "Prop.hpp"

#include <iostream>

void print_matrix(const glm::mat4 &matrix)
{
  std::cout << "{ " << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << ", " << matrix[0][3] << " }\n";
  std::cout << "{ " << matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << ", " << matrix[1][3] << " }\n";
  std::cout << "{ " << matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << ", " << matrix[2][3] << " }\n";
  std::cout << "{ " << matrix[3][0] << ", " << matrix[3][1] << ", " << matrix[3][2] << ", " << matrix[3][3] << " }" << std::endl;
}

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
  trns = trns * glm::mat4_cast(glm::quat({ rot.x, rot.y, rot.z }));
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
