#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>

struct Mesh
{
  std::vector<glm::vec3> vertices;
  //std::vector<glm::vec2> uvs;
  std::vector<GLushort>  indicies;

  glm::mat4 object2world;

  void upload(GLint pos_uniform_loc)
  {
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(pos_uniform_loc);
    glVertexAttribPointer(pos_uniform_loc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
    //                      sizeof(vertices[0]), (void *)(sizeof(float) * 2));
  }

private:
  GLuint vertex_buffer, vertex_array;
};