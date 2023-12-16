#pragma once

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) \
 || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#include <windows.h>
#endif

#define PROJECT_NAME "portal"

#include "Mesh.hpp"

#include <glm/glm.hpp>

// portal.cpp
Mesh create_portal_mesh();
glm::mat4 portal_view(glm::mat4 orig_view, Mesh *src, Mesh *dst);
