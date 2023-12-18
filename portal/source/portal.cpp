#include "portal.hpp"

#include "Mesh.hpp"
#include "Image.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "Texture.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

// portal 
//static glm::mat4 portal_view(glm::mat4 orig_view, Mesh *src, Mesh *dst)
//{
//  glm::mat4 mv = orig_view * src->transform;
//  glm::mat4 portal_cam =
//    // 3. transformation from source portal to the camera - it's the
//    //    first portal's ModelView matrix:
//    mv
//    // 2. object is front-facing, the camera is facing the other way:
//    * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0))
//    // 1. go the destination portal; using inverse, because camera
//    //    transformations are reversed compared to object
//    //    transformations:
//    * glm::inverse(dst->transform)
//    ;
//  return portal_cam;
//}


Mesh create_portal_mesh()
{
  Mesh portal;

  const glm::vec3 portal_vertices[] = {
    glm::vec3( 1, 0,  1),
    glm::vec3(-1, 0,  1),
    glm::vec3( 1, 0, -1),
    glm::vec3(-1, 0, -1),
  };

  const glm::vec2 portal_uvs[] = {
    glm::vec2(0, 0),
    glm::vec2(1, 0),
    glm::vec2(0, 1),
    glm::vec2(1, 1),
  };

  const GLushort portal_elements[] = {
    0,1,2, 2,1,3,
  };

  for (unsigned int i = 0; i < sizeof(portal_vertices) / sizeof(portal_vertices[0]); i++)
  {
    portal.vertices.push_back(portal_vertices[i]);
    portal.uvs.push_back(portal_uvs[i]);
  }

  for (unsigned int i = 0; i < sizeof(portal_elements) / sizeof(portal_elements[0]); i++)
    portal.indicies.push_back(portal_elements[i]);

  portal.upload();

  return portal;
}
