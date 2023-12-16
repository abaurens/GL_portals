#include "OBJLoader.hpp"

#include <fstream>
#include <iostream>

#include <glm/glm.hpp>

struct OBJData
{
  struct Face
  {
    glm::ivec3 vertices[3];
  };

  std::vector<glm::vec3>  positions;
  std::vector<glm::vec3>  normals;
  std::vector<glm::vec2>  uvs;
  std::vector<Face>       faces;

  void clear()
  {
    faces.clear();

    uvs.clear();
    normals.clear();
    positions.clear();
  }
};

#include <stdarg.h>
#define vscprintf _vscprintf


int vasprintf(char **strp, const char *format, va_list ap)
{
  int len = vscprintf(format, ap);
  if (len == -1)
    return -1;
  char *str = (char *)malloc((size_t)len + 1);
  if (!str)
    return -1;
  int retval = vsnprintf(str, len + 1, format, ap);
  if (retval == -1) {
    free(str);
    return -1;
  }
  *strp = str;
  return retval;
}

int asprintf(char **strp, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  int retval = vasprintf(strp, format, ap);
  va_end(ap);
  return retval;
}




// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}






static bool parse_face(OBJData &mesh, const std::string_view data)
{
  std::vector<std::string_view> texts;
  OBJData::Face face;

  size_t i = 0;
  size_t end = 0;
  while (i != std::string::npos)
  {
    end = data.find_first_of(" ", i);
    texts.push_back(data.substr(i, end - i));
    i = end + (end != std::string::npos);
  }

  if (texts.size() != 3)
  {
    std::cout << "Error: face is not a triangle: '" << data << "'" << std::endl;
    return false;
  }

  for (uint32_t i = 0; i < 3; ++i)
  {
    int32_t pos = 0;
    int32_t uv = 0;
    int32_t norm = 0;

    auto _ = sscanf(texts[i].data(), "%d/%d/%d", &pos, &uv, &norm);

    if (pos == 0 || pos > mesh.positions.size() && -pos > mesh.positions.size())
    {
      std::cout << "Error: Face references a non existent position (" << pos << ")" << std::endl;
      return false;
    }

    if (uv != 0 && (uv < 0 || uv > mesh.uvs.size()))
    {
      std::cout << "Error: Face references a non existent tecture coordinate (" << uv << ")" << std::endl;
      return false;
    }
    
    if (norm != 0 && (norm < 0 || norm > mesh.normals.size()))
    {
      std::cout << "Error: Face references a non existent normal (" << norm << ")" << std::endl;
      return false;
    }

    if (pos < 0)
      pos = ((uint32_t)mesh.positions.size() + pos + 1);

    face.vertices[i] = { pos, uv, norm };
  }

  mesh.faces.push_back(face);
  return true;
}

static bool parseLine(OBJData &mesh, const std::string_view type, const std::string_view data)
{
  // unsuported features
  if (type == "g")
    return true;
  if (type == "s")
    return true;
  if (type == "l")
    return true;
  if (type == "vp")
    return true;

  // positions
  if (type == "v")
  {
    glm::vec3 pos;
    auto _ = sscanf(data.data(), "%f %f %f", &pos.x, &pos.y, &pos.z);
    mesh.positions.push_back(std::move(pos));
    return true;
  }

  // normals
  if (type == "vn")
  {
    glm::vec3 norm;
    auto _ = sscanf(data.data(), "%f %f %f", &norm.x, &norm.y, &norm.z);
    mesh.normals.push_back(std::move(norm));
    return true;
  }

  if (type == "vt")
  {
    glm::vec2 uv;
    auto _ = sscanf(data.data(), "%f %f", &uv.x, &uv.y);
    uv.y = -uv.y;
    mesh.uvs.push_back(std::move(uv));
    return true;
  }

  if (type == "f")
    return parse_face(mesh, data);

  return false;
}



static Mesh toMesh(OBJData &data)
{
  std::cout << "Mesh loaded:\n"
    << "  " << data.positions.size() << " vertices\n"
    << "  " << data.uvs.size() << " texture coordinates\n"
    << "  " << data.normals.size() << " normals\n"
    << "  " << data.faces.size() << " faces" << std::endl;

  Mesh mesh;
  mesh.vertices = std::move(data.positions);

  // allocate memory
  if (!data.uvs.empty())
    mesh.uvs.resize(mesh.vertices.size());
  //if (!data.normals.empty())
  //  mesh.normals.resize(mesh.vertices.size());
  mesh.indicies.reserve(data.faces.size() * 3);

  //std::cout << "allocated space for " << mesh.uvs.size() << " UVs" << std::endl;
  //std::cout << "allocated space for " << 0 << " normals" << std::endl;
  //std::cout << "allocated space for " << (data.faces.size() * 3) << " indicies" << std::endl;

  // function for one vertex
  auto storeVertex = [&mesh, &data](glm::ivec3 indicies)
  {
    // x is vertex index
    const uint32_t index = indicies.x - 1;
    mesh.indicies.push_back(index);

    // y is uv index
    if (indicies.y != 0)
      mesh.uvs[index] = data.uvs[indicies.y - 1];

    // z is normal index
    //if (indicies.z)
    //  mesh.normals[index] = data.normals[indicies.z - 1]
  };

  for (const OBJData::Face &face : data.faces)
  {
    storeVertex(face.vertices[0]);
    storeVertex(face.vertices[1]);
    storeVertex(face.vertices[2]);
  }

  std::cout << "Mesh build:\n"
    << " " << mesh.vertices.size() << " vertices\n"
    << " " << mesh.uvs.size() << " uvs\n"
    //<< " " << mesh.normals.size() << " normals\n"
    << " " << mesh.indicies.size() << " indicies" << std::endl;

  mesh.upload();
  return mesh;
}

std::vector<Mesh> OBJ::load(const std::filesystem::path &path)
{
  if (!std::filesystem::exists(path))
  {
    std::cout << "OBJ::load error: " << path << " doesnt exist." << std::endl;
    abort();
  }

  std::ifstream ifs;
  ifs.open(path);
  if (!ifs)
  {
    std::cout << "OBJ::load error: " << path << " is not accessible" << std::endl;
    abort();
  }

  OBJData obj;
  std::vector<Mesh> meshes;

  obj.clear();

  std::string line;
  while (std::getline(ifs, line))
  {
    size_t end = line.find_first_of('#');
    if (end != std::string::npos)
      line.erase(end);
    trim(line);

    if (line.empty())
      continue;

    end = line.find_first_of(' ');
    std::string data = line;
    std::string type = data.substr(0, end);
    data.erase(0, std::max(end, end + 1));

    if (type == "o")
    {
      if (!obj.faces.empty())
      {
        meshes.push_back(toMesh(obj));
      }
      obj.clear();
    }
    else if (!parseLine(obj, type, data))
    {
      std::cout << "Unrecognized OBJ line (ignored): " << line << std::endl;
      continue;
    }
  }

  if (!obj.faces.empty())
    meshes.push_back(toMesh(obj));

  return meshes;
}