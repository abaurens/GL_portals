#pragma once

#include "Mesh.hpp"

#include <map>
#include <string>
#include <filesystem>

namespace OBJ
{
  std::map<std::string, Mesh> load(const std::filesystem::path &path, const std::string &defaultName = "");
}