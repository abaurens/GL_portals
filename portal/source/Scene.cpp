#include "Scene.hpp"

#include "portal.hpp"

#include "OBJLoader.hpp"

void Scene::init()
{
  // load shaders
  flat.addFile(Shader::Type::vertex, "./shaders/flat.vert");
  flat.addFile(Shader::Type::fragment, "./shaders/flat.frag");
  flat.compile();

  flat_tex.addFile(Shader::Type::vertex, "./shaders/flat_texture.vert");
  flat_tex.addFile(Shader::Type::fragment, "./shaders/flat_texture.frag");
  flat_tex.compile();


  // load textures
  Image img;
  img.load("./assets/test.png", 3);
  texture.upload(img);

  img.load("./assets/metal_box.png", 3);
  storageCubeTexture.upload(img);


  // generate materials
  flat_tex_mat.m_shader = flat_tex;
  flat_tex_mat.m_textures.push_back(texture);

  flat_white_mat.m_shader = flat;

  storageCube_mat.m_shader = flat_tex;
  storageCube_mat.m_textures.push_back(storageCubeTexture);

  // generate meshes
  meshes.push_back(create_portal_mesh());
  meshes.push_back(create_portal_mesh());

  {
    std::vector<Mesh> loaded;
    loaded = OBJ::load("./assets/StorageCube.obj");
    for (Mesh &m : loaded)
      m.transform = glm::scale(glm::mat4(1), glm::vec3(1.0f / 50.0f));
    meshes.insert(meshes.end(), std::make_move_iterator(loaded.begin()), std::make_move_iterator(loaded.end()));
    loaded.erase(loaded.begin());
  }

  meshes[0].setMaterial(&flat_tex_mat);
  meshes[2].setMaterial(&storageCube_mat);

  // 90° angle + slightly higher
  meshes[0].transform = glm::translate(glm::mat4(1), glm::vec3(0, -2, -1));
  meshes[1].transform = glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 0, 1)) * glm::translate(glm::mat4(1), glm::vec3(0, -2, -1.2));
}

void Scene::cleanup()
{
  texture.clear();

  flat.clear();
  flat_tex.clear();

  for (Mesh &mesh : meshes)
    mesh.clear();
  meshes.clear();
}

void Scene::render(const Camera &camera)
{
  for (const Mesh &mesh : meshes)
    mesh.render(camera);
}