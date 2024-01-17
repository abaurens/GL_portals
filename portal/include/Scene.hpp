#pragma once

#include "portal.hpp"

#include "Mesh.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Material.hpp"

#include "Prop.hpp"

#include <nlohmann/json.hpp>

#include <map>
#include <set>
#include <filesystem>

class Scene
{
public:
  ~Scene() { cleanup(); };

  bool reload();
  bool load(const std::filesystem::path &path);
  void cleanup();

  void updateUI();
  void render(const Camera &camera) const;

private:
  void load_meshes(const nlohmann::json &json_meshes);
  void parse_mesh(const std::string &name, const nlohmann::json &json_mesh);

  void load_shaders(const nlohmann::json &json_shaders);
  void parse_shader(const std::string &name, const nlohmann::json &json_shader);

  void load_textures(const nlohmann::json &json_textures);
  void parse_texture(const std::string &name, const nlohmann::json &json_texture);

  void load_materials(const nlohmann::json &json_materials);
  void parse_material(const std::string &name, const nlohmann::json &json_texture);

  void load_scene(const nlohmann::json &json_scene);
  void parse_prop(const std::string &name, const nlohmann::json &json_prop);

  std::filesystem::path m_sceneFile;

  std::map<std::string, Mesh> meshes;
  std::map<std::string, Shader> shaders;
  std::map<std::string, Texture> textures;
  std::map<std::string, Material> materials;

  std::string m_selected_prop = "";
  std::map<std::string, Prop> props;
  std::set<uint32_t> propIds;
};