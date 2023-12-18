#include "portal.hpp"

#include "Mesh.hpp"

#include <glm/glm.hpp>

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
