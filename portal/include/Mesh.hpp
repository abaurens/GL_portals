#pragma once

#include "Shader.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>

struct Mesh
{
  std::vector<glm::vec3> vertices;
  //std::vector<glm::vec2> uvs;
  std::vector<GLushort>  indicies;

  glm::mat4 transform;

  void upload(const Shader &shader)
  {
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    GLint vPos_loc = shader.getAttribute("vPos");
    glEnableVertexAttribArray(vPos_loc);
    glVertexAttribPointer(vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
    //                      sizeof(vertices[0]), (void *)(sizeof(float) * 2));
  }

  void clear()
  {
    if (glIsBuffer(vertex_buffer))
      glDeleteBuffers(1, &vertex_buffer);
    
    if (glIsVertexArray(vertex_array))
      glDeleteVertexArrays(1, &vertex_array);

    vertices.clear();
    indicies.clear();
    transform = glm::mat4(1);

    vertex_buffer = 0;
    vertex_array = 0;
  }

private:
  GLuint vertex_buffer, vertex_array;
};