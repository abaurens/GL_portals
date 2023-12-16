#pragma once

#include "Mesh.hpp"

#include <vector>
#include <filesystem>

namespace OBJ
{
  std::vector<Mesh> load(const std::filesystem::path &path);
}