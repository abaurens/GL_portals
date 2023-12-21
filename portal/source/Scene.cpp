#include "portal.hpp"
#include "Scene.hpp"
#include "OBJLoader.hpp"
#include "StringUtils.hpp"

#include "imgui.h"

#include <fstream>
#include <iostream>


using json = nlohmann::json;

bool Scene::reload()
{
  return load(m_sceneFile);
}

bool Scene::load(const std::filesystem::path &path)
{
  if (!std::filesystem::exists(path))
  {
    std::cout << "Scene loading error: " << path << " doesn't exist." << std::endl;
    return false;
  }

  std::ifstream ifs(path);
  if (!ifs)
  {
    std::cout << "Scene loading error: " << path << " is not accessible." << std::endl;
    return false;
  }

  json sceneFile;
  try {
    sceneFile = json::parse(ifs, nullptr, true, true);
  } catch (const json::parse_error &e) {
    std::cout << "Scene loading error: JSON error\n"
      << "  " << e.what() << std::endl;
  }

  props.clear();

  auto it = sceneFile.find("name");
  if (it != sceneFile.end())
    std::cout << "Scene name: " << sceneFile["name"] << std::endl;

  it = sceneFile.find("meshes");
  if (it != sceneFile.end())
    load_meshes(*it);

  it = sceneFile.find("shaders");
  if (it != sceneFile.end())
    load_shaders(*it);

  it = sceneFile.find("textures");
  if (it != sceneFile.end())
    load_textures(*it);

  it = sceneFile.find("materials");
  if (it != sceneFile.end())
    load_materials(*it);

  it = sceneFile.find("scene");
  if (it != sceneFile.end())
    load_scene(*it);

  m_sceneFile = path;
  return true;
}

void Scene::cleanup()
{
  for (auto &[name, mesh] : meshes)
    mesh.clear();

  for (auto &[name, shader] : shaders)
    shader.clear();

  for (auto &[name, texture] : textures)
    texture.clear();

  props.clear();
  meshes.clear();
  shaders.clear();
  textures.clear();
  materials.clear();
}

void Scene::updateUI()
{
  bool first;
  if (ImGui::Begin("Scene", 0, ImGuiWindowFlags_None & ImGuiWindowFlags_NoTitleBar))
  {
    if (ImGui::Button("Reload scene"))
      reload();

    if (ImGui::TreeNode("Shaders"))
    {
      first = true;
      for (const auto &[name, _] : shaders)
      {
        if (!first)
          ImGui::Separator();
        ImGui::PushID(name.c_str());
        ImGui::Text("%s", name.c_str());
        ImGui::PopID();
        first = false;
      }

      ImGui::TreePop();
      ImGui::Spacing();
    }
    ImGui::Separator();

    if (ImGui::TreeNode("Meshes"))
    {
      first = true;
      for (const auto &[name, mesh] : meshes)
      {
        if (!first)
          ImGui::Separator();
        ImGui::PushID(name.c_str());
        ImGui::Text("%s", name.c_str());
        ImGui::PopID();
        first = false;
      }

      ImGui::TreePop();
      ImGui::Spacing();
    }
    ImGui::Separator();

    if (ImGui::TreeNode("textures"))
    {
      first = true;
      for (const auto &[name, texture] : textures)
      {
        if (!first)
          ImGui::Separator();
        ImGui::PushID(name.c_str());
        ImGui::Text("%s", name.c_str());
        ImGui::Image((ImTextureID)(uintptr_t)texture, ImVec2{ 256, 256 });
        ImGui::PopID();
        first = false;
      }

      ImGui::TreePop();
      ImGui::Spacing();
    }
    ImGui::Separator();

    if (ImGui::TreeNode("Materials"))
    {
      first = true;
      for (const auto &[name, material] : materials)
      {
        if (!first)
          ImGui::Separator();
        ImGui::PushID(name.c_str());
        ImGui::Text("%s", name.c_str());
        //ImGui::Text("shader: %s", material.m_shader);
        ImGui::PopID();
        first = false;
      }

      ImGui::TreePop();
      ImGui::Spacing();
    }
    ImGui::Separator();

    if (ImGui::TreeNode("Props"))
    {
      ImGuiTreeNodeFlags base_flags = 
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth |
        ImGuiTreeNodeFlags_SpanFullWidth |
        ImGuiTreeNodeFlags_Leaf |
        ImGuiTreeNodeFlags_NoTreePushOnOpen;

      std::string clicked_prop;
      for (auto &[name, prop] : props)
      {
        ImGuiTreeNodeFlags leaf_flags = base_flags;
        if (name == m_selected_prop)
          leaf_flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::TreeNodeEx(name.c_str(), leaf_flags, "%s", name.c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
          clicked_prop = name;
      }

      if (!clicked_prop.empty())
      {
        if (ImGui::GetIO().KeyCtrl && clicked_prop == m_selected_prop)
          m_selected_prop = nullptr;
        else
          m_selected_prop = clicked_prop;
      }

      ImGui::TreePop();
      ImGui::Spacing();
    }
  }
  ImGui::End();

  ImGui::Begin("Properties");
  if (!m_selected_prop.empty())
  {
    Prop &prop = props[m_selected_prop];

    ImGui::DragFloat3("position", &prop.pos.x, 0.001f);

    glm::vec3 rot = glm::degrees(glm::eulerAngles(prop.rot));
    ImGui::DragFloat3("rotation", &rot.x, 1.0f);
    prop.rot = glm::quat(glm::radians(rot));

    ImGui::DragFloat3("scale", &prop.scale.x, 0.001f);
  }
  ImGui::End();
}

void Scene::render(const Camera &camera) const
{
  for (const auto &[name, prop] : props)
    prop.render(camera);
}


// loaders
void Scene::load_meshes(const json &json_meshes)
{
  for (auto it = json_meshes.begin(); it != json_meshes.end(); ++it)
    parse_mesh(it.key(), *it);
}

void Scene::parse_mesh(const std::string &name, const json &json_mesh)
{
  if (json_mesh.is_string())
  {
    std::map<std::string, Mesh> &&loaded = OBJ::load("./assets/meshes/" + json_mesh.get<std::string>(), name);

    for (auto &[found_name, mesh] : loaded)
    {
      if (loaded.size() == 1)
        meshes.insert_or_assign(name, std::move(mesh));
      else
        meshes.insert_or_assign(found_name, std::move(mesh));
    }

    loaded.erase(loaded.begin(), loaded.end());
    return;
  }

  if (json_mesh.is_object())
  {
    if (!json_mesh.contains("positions"))
    {
      std::cout << "Scene loading error :"
        << " missing 'position' field for mesh \"" << name << "\" definition"
        << " (ignored)" << std::endl;
      return;
    }

    if (!json_mesh.contains("indicies"))
    {
      std::cout << "Scene loading error :"
        << " missing 'indicies' field for mesh \"" << name << "\" definition"
        << " (ignored)" << std::endl;
      return;
    }

    Mesh mesh;
    bool err = false;

    json json_pos = json_mesh["positions"];
    if (!json_pos.is_array())
    {
      err = true;
      std::cout << "Scene loading error :"
        << " Format error on 'position' field of \"" << name << "\" mesh definition." << json_mesh
        << " (mesh ignored)" << std::endl;
      return;
    }

    for (auto it = json_pos.begin(); it != json_pos.end(); ++it)
    {
      json json_vec3 = *it;

      if (!json_vec3.is_array() || json_vec3.size() != 3)
      {
        std::cout << "Scene loading error :"
          << " Invalid vector for 'position' field of mesh \"" << name << "\" definition"
          << " (ignored)" << std::endl;
        return;
      }

      glm::vec3 position;
      for (uint32_t i = 0; i < 3; ++i)
        position[i] = json_vec3[i].get<float>();

      mesh.vertices.push_back(position);
    }

    json json_ind = json_mesh["indicies"];
    if (!json_ind.is_array())
    {
      std::cout << "Scene loading error :"
        << " Format error on 'indicies' field of \"" << name << "\" mesh definition." << json_mesh
        << " (mesh ignored)" << std::endl;
    }

    for (auto it = json_ind.begin(); it != json_ind.end(); ++it)
    {
      GLuint ind = it->get<GLuint>();
      mesh.indicies.push_back(ind);
    }

    if (mesh.indicies.size() % 3 != 0)
    {
      std::cout << "Scene loading warning :"
        << " Indicies count is not a multiple of 3 for \"" << name << "\" mesh definition." << json_mesh
        << " (only " << (mesh.indicies.size() / 3) << " will be used)" << std::endl;
    }

    if (json_mesh.contains("uvs"))
    {
      json json_uvs = json_mesh["uvs"];
      if (!json_uvs.is_array())
      {
        std::cout << "Scene loading error :"
          << " Format error on 'uvs' field of \"" << name << "\" mesh definition." << json_mesh
          << " (mesh ignored)" << std::endl;
      }
      for (auto it = json_uvs.begin(); it != json_uvs.end(); ++it)
      {
        json json_vec2 = *it;

        if (!json_vec2.is_array() || json_vec2.size() != 2)
        {
          std::cout << "Scene loading error :"
            << " Invalid vector for 'uvs' field of mesh \"" << name << "\" definition"
            << " (ignored)" << std::endl;
          return;
        }

        glm::vec2 uv;
        for (uint32_t i = 0; i < 2; ++i)
          uv[i] = json_vec2[i].get<float>();

        mesh.uvs.push_back(uv);
      }

      if (mesh.uvs.size() < mesh.vertices.size())
      {
        std::cout << "Scene loading warning :"
          << " Not enough UVs in \"" << name << "\" mesh definition." << json_mesh
          << " (missing UVs filled with {0, 0})" << std::endl;
        for (size_t i = mesh.uvs.size(); i < mesh.vertices.size(); ++i)
          mesh.uvs.push_back({ 0.0f, 0.0f });
      }
    }

    if (json_mesh.contains("normals"))
    {}

    if (mesh.vertices.size() && mesh.indicies.size())
    {
      mesh.upload();
      meshes.insert_or_assign(name, std::move(mesh));
      return;
    }
  }

  std::cout << "Scene loading error :"
    << " Invalid mesh definition: \"" << name << "\": " << json_mesh
    << " (ignored)" << std::endl;
}


void Scene::load_shaders(const json &json_shaders)
{
  for (auto it = json_shaders.begin(); it != json_shaders.end(); ++it)
  {
    parse_shader(it.key(), *it);
  }
}

void Scene::parse_shader(const std::string &name, const json &json_shader)
{
  Shader::Type present_types = Shader::Type::none;

  using enum Shader::Type;

  if (json_shader.is_object())
  {
    if (json_shader.contains("vertex"))
      present_types |= vertex;
    if (json_shader.contains("fragment"))
      present_types |= fragment;
    if (json_shader.contains("compute"))
      present_types |= compute;
    if (json_shader.contains("geometry"))
      present_types |= geometry;
    if (json_shader.contains("tesselation"))
      present_types |= tesselation;

    Shader shader;
    bool success = true;
    bool unsuported_msg_printed = false;
    for (auto it = json_shader.begin(); it != json_shader.end(); ++it)
    {
      Shader::Type current_type = Shader::typeFromString(it.key());

      if (current_type == Shader::Type::none)
      {
        std::cout << "Scene loading error :"
          << " Unknown shader type: \"" << it.key() << " (ignored)" << std::endl;
        continue;
      }

      // compute, geometry, and tesselation shaders are not supported
      if (!!(present_types & (compute | geometry | tesselation)))
      {
        if (!unsuported_msg_printed)
        {
          unsuported_msg_printed = true;
          std::cout << "Scene loading warning :"
            << " shader other than vertex and fragment are not supported yet: \"" << name << "\": " << json_shader
            << " (ignored)" << std::endl;
        }
        continue;
      }

      present_types |= current_type;

      if (it->is_string())
      {
        std::string value = *it;
        trim(value);

        if (value.starts_with("#version"))
          success &= shader.addSource(current_type, value);
        else
          success &= shader.addFile(current_type, "./assets/shaders/" + value);
        continue;
      }
      
      if (it->is_array() && (*it)[0].is_string())
      {
        std::vector<std::string_view> sources;
        for (uint32_t i = 0; i < it->size(); ++i)
          sources.push_back((*it)[i]);
        success &= shader.addSource(current_type, sources);
        continue;
      }

      std::cout << "Scene loading error :"
        << " Invalid shader format: \"" << name << "\": " << json_shader
        << " (ignored)" << std::endl;
      continue;
    }

    if (success)
      success &= shader.compile();

    if (success)
      shaders.insert_or_assign(name, std::move(shader));

    return;
  }

  std::cout << "Scene loading error :"
    << " Invalid shader definition: \"" << name << "\": " << json_shader
    << " (ignored)" << std::endl;
}


void Scene::load_textures(const json &json_textures)
{
  for (auto it = json_textures.begin(); it != json_textures.end(); ++it)
    parse_texture(it.key(), *it);
}

void Scene::parse_texture(const std::string &name, const json &json_texture)
{
  Image img;

  if (json_texture.is_string())
  {
    img.load("./assets/textures/" + json_texture.get<std::string>(), 3);
    textures[name].upload(img);
    return;
  }

  std::cout << "Scene loading error :"
    << " Invalid texture definition: \"" << name << "\": " << json_texture
    << " (ignored)" << std::endl;
}


void Scene::load_materials(const json &json_materials)
{
  for (auto it = json_materials.begin(); it != json_materials.end(); ++it)
    parse_material(it.key(), *it);
}

void Scene::parse_material(const std::string &name, const json &json_material)
{
  Material material;

  const auto parse_material_shader = [this, &name, &json_material, &material]() -> bool
  {
    if (!json_material.contains("shader"))
    {
      std::cout << "Scene loading error :"
        << " Missing shader in material: \"" << name << "\""
        << " (ignored)" << std::endl;
      return false;
    }

    const std::string &shader = json_material["shader"];
    if (!shaders.contains(shader))
    {
      std::cout << "Scene loading error :"
        << " Material \"" << name << "\" references an unknown shader: \"" << json_material["shader"]
        << " (ignored)" << std::endl;
      return false;
    }

    material.m_shader = &shaders[shader];
    return true;
  };

  const auto parse_material_texture = [this, &name, &json_material, &material]() -> bool
  {
    if (!json_material.contains("textures"))
      return true;

    json tex = json_material["textures"];

    if (tex.is_array())
    {
      for (auto it = tex.begin(); it != tex.end(); ++it)
      {
        const std::string &tex_name = it->get<std::string>();

        if (!textures.contains(tex_name))
        {
          std::cout << "Scene loading error :"
            << " Material \"" << name << "\" references an unknown texture: \"" << tex_name << "\""
            << " (ignored)" << std::endl;
          return false;
        }
        material.m_textures.push_back(&textures[tex_name]);
      }
      return true;
    }

    std::cout << "Scene loading error :"
      << " Invalid texture reference in material: \"textures\": " << tex
      << " (ignored)" << std::endl;
    return false;
  };

  if (json_material.is_object())
  {
    if (parse_material_shader())
    {
      parse_material_texture();
      materials[name].m_shader = material.m_shader;
      materials[name].m_textures = material.m_textures;
    }
    return;
  }

  std::cout << "Scene loading error :"
    << " Invalid material definition: \"" << name << "\": " << json_material
    << " (ignored)" << std::endl;
}


void Scene::load_scene(const json &json_scene)
{
  for (auto it = json_scene.begin(); it != json_scene.end(); ++it)
    parse_prop(it.key(), *it);
}

void Scene::parse_prop(const std::string &name, const json &json_prop)
{
  if (!json_prop.is_object())
  {
    std::cout << "Scene loading error :"
      << " Invalid prop definition: \"" << name << "\": " << json_prop
      << " (ignored)" << std::endl;
    return;
  }

  Prop prop;

  auto it = json_prop.find("mesh");
  if (it != json_prop.end())
  {
    if (!it->is_string())
    {
      std::cout << "Scene loading error :"
        << " format error on 'mesh' field of prop \"" << name << "\" definition"
        << " (ignored)" << std::endl;
    }
    else
    {
      const std::string &mesh = it->get<std::string>();
      if (!meshes.contains(mesh))
      {
        std::cout << "Scene loading error :"
          << " Prop \"" << name << "\" references an unknown mesh: \"" << mesh
          << " (ignored)" << std::endl;
      }
      else
        prop.mesh = &meshes[mesh];
    }
  }

  it = json_prop.find("material");
  if (it != json_prop.end())
  {
    if (!it->is_string())
    {
      std::cout << "Scene loading error :"
        << " format error on 'material' field of prop \"" << name << "\" definition"
        << " (ignored)" << std::endl;
    }
    else
    {
      const std::string &material = it->get<std::string>();
      if (!materials.contains(material))
      {
        std::cout << "Scene loading error :"
          << " Prop \"" << name << "\" references an unknown material: \"" << material
          << "\" (ignored)" << std::endl;
      }
      else
        prop.material = &materials[material];
    }
  }

  it = json_prop.find("position");
  if (it != json_prop.end())
  {
    if (!it->is_array() || (it->size() != 1 && it->size() != 3))
    {
      std::cout << "Scene loading error :"
        << " format error on 'position' field of prop \"" << name << "\" definition"
        << " (ignored)" << std::endl;
    }
    else
    {
      glm::vec3 pos;
      for (uint32_t i = 0; i < 3; ++i)
        pos[i] = (*it)[i % it->size()].get<float>();
      prop.pos = pos;
    }
  }

  it = json_prop.find("rotation");
  if (it != json_prop.end())
  {
    if (!it->is_array() || (it->size() != 1 && it->size() != 3))
    {
      std::cout << "Scene loading error :"
        << " format error on 'rotation' field of prop \"" << name << "\" definition"
        << " (ignored)" << std::endl;
    }
    else
    {
      glm::vec3 rot;
      for (uint32_t i = 0; i < 3; ++i)
        rot[i] = (*it)[i % it->size()].get<float>();

      rot = glm::radians(rot);
      prop.rot = glm::quat(rot);
    }
  }

  it = json_prop.find("scale");
  if (it != json_prop.end())
  {
    if (!it->is_array() || (it->size() != 1 && it->size() != 3))
    {
      std::cout << "Scene loading error :"
        << " format error on 'scale' field of prop \"" << name << "\" definition"
        << " (ignored)" << std::endl;
    }
    else
    {
      glm::vec3 scale;
      for (uint32_t i = 0; i < 3; ++i)
        scale[i] = (*it)[i % it->size()].get<float>();
      prop.scale = scale;
    }
  }

  props[name] = prop;
}