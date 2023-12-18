#include "Shader.hpp"

#include <vector>
#include <fstream>
#include <iostream>


static constexpr std::string_view default_vertex = R"(
  #version 110
  
  uniform mat4 MVP;
  attribute vec3 vPos;
  
  void main()
  {
    gl_Position = MVP * vec4(vPos, 1.0);
  }
)";

static constexpr std::string_view default_fragment = R"(
  #version 110
  
  void main()
  {
    gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
  }
)";

Shader Shader::default_shader;

const Shader &Shader::getDefaultShader()
{
  if (!default_shader.valid())
  {
    default_shader.addSource(Type::vertex, default_vertex);
    default_shader.addSource(Type::fragment, default_fragment);
    default_shader.compile();
  }

  if (!default_shader.valid())
    abort();
  return default_shader;
}



Shader::Shader() noexcept : m_valid(false), m_program(0) {}

Shader::Shader(Shader &&mv) noexcept :
  m_valid(mv.m_valid),
  m_program(mv.m_program),
  m_shaders(std::move(mv.m_shaders)),
  m_uniforms(std::move(mv.m_uniforms)),
  m_attributes(std::move(mv.m_attributes))
{
  mv.m_valid = false;
  mv.m_program = 0;
  mv.m_shaders.clear();
  mv.m_uniforms.clear();
  mv.m_attributes.clear();
}

Shader &Shader::operator=(Shader &&other) noexcept
{
  m_valid = other.m_valid;
  m_program = other.m_program;
  m_shaders = std::move(other.m_shaders);
  m_uniforms = std::move(other.m_uniforms);
  m_attributes = std::move(other.m_attributes);

  other.m_valid = false;
  other.m_program = 0;
  other.m_shaders.clear();
  other.m_uniforms.clear();
  other.m_attributes.clear();

  return *this;
}


bool Shader::addSource(Type type, const std::vector<std::string_view> &source)
{
  GLuint shaderId = getOrCreate(type);

  // compile the source code
  const char **src = new const char *[source.size() + 1];

  for (uint32_t i = 0; i < source.size(); ++i)
    src[i] = source[i].data();
  src[source.size()] = nullptr;

  glShaderSource(shaderId, (GLsizei)source.size(), src, NULL);
  glCompileShader(shaderId);

  // error management
  GLint result;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
  if (result != GL_TRUE)
  {
    getShaderErrors(shaderId);
    return false;
  }

  m_valid = false;
  return true;
}


bool Shader::addSource(Type type, const std::string_view &source)
{
  GLuint shaderId = getOrCreate(type);

  // compile the source code
  const char *src = source.data();
  glShaderSource(shaderId, 1, &src, NULL);
  glCompileShader(shaderId);

  // error management
  GLint result;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
  if (result != GL_TRUE)
  {
    getShaderErrors(shaderId);
    return false;
  }

  m_valid = false;
  return true;
}

bool Shader::addFile(Type type, const std::filesystem::path &filePath)
{
  std::stringstream buffer;

  if (!std::filesystem::exists(filePath))
  {
    std::cout << "Shader compile error: " << filePath << " does not exist or is inaccessible." << std::endl;
    return false;
  }

  std::ifstream ifs(filePath);
  if (!ifs)
  {
    std::cout << "Shader compile error: " << strerror(errno) << std::endl;
    ifs.close();
    return false;
  }

  buffer << ifs.rdbuf();
  ifs.close();

  return addSource(type, buffer.str());
}

void Shader::remove(Type type)
{
  if (m_shaders.contains(type))
  {
    const GLuint shaderId = m_shaders.at(type);

    m_shaders.erase(type);
    if (glIsShader(shaderId))
    {
      m_valid = false;
      glDeleteShader(shaderId);
    }
  }
}

void Shader::clear()
{
  for (auto &[_, shaderId] : m_shaders)
    if (glIsShader(shaderId))
      glDeleteShader(shaderId);

  m_shaders.clear();

  if (glIsProgram(m_program))
    glDeleteProgram(m_program);

  m_program = 0;
  m_valid = false;
}

bool Shader::compile()
{
  if (!glIsProgram(m_program))
    m_program = glCreateProgram();

  for (auto &[type, shader] : m_shaders)
    if (glIsShader(shader))
      glAttachShader(m_program, shader);

  glLinkProgram(m_program);

  GLint result;
  glGetProgramiv(m_program, GL_LINK_STATUS, &result);
  if (result != GL_TRUE)
    getProgramErrors();

  m_valid = result == GL_TRUE;

  for (auto &[type, shader] : m_shaders)
    if (glIsShader(shader))
      glDetachShader(m_program, shader);

  if (m_valid)
  {
    scanUniforms();
    scanAttributes();
  }

  return m_valid;
}

GLuint Shader::id()
{
  if (!m_valid)
    compile();

  return m_valid ? m_program : 0;
}

GLuint Shader::id() const
{
  return m_valid ? m_program : 0;
}

GLint Shader::getUniform(const std::string &name) const
{
  if (!m_uniforms.contains(name))
    return -1;
  return m_uniforms.at(name);
}

GLint Shader::getAttribute(const std::string &name) const
{
  if (!m_attributes.contains(name))
    return -1;
  return m_attributes.at(name);
}


Shader::Type Shader::typeFromString(const std::string_view name)
{
  if (name == "vertex")
    return Type::vertex;
  if (name == "fragment")
    return Type::fragment;
  if (name == "compute")
    return Type::compute;
  if (name == "geometry")
    return Type::geometry;
  if (name == "tesselation")
    return Type::tesselation;
  return Type::none;
}

GLenum Shader::typeToGl(Type type)
{
  switch (type)
  {
  case Type::vertex:
    return GL_VERTEX_SHADER;
  case Type::fragment:
    return GL_FRAGMENT_SHADER;
  case Type::compute:
    return GL_COMPUTE_SHADER;
  case Type::geometry:
    return GL_GEOMETRY_SHADER;
  case Type::tesselation:
    return GL_TESS_EVALUATION_SHADER;
  default:
    return 0;
  }
}



void Shader::scanUniforms()
{
  GLint uniforms;
  GLint longestUniform;

  GLint   size;
  GLenum  type;
  GLsizei length;
  std::string name;
  std::vector<char> buffer;

  glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &uniforms);
  glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &longestUniform);

  buffer.resize(longestUniform);
  
  //std::cout << "Found : " << uniforms << " uniforms" << std::endl;
  for (GLint i = 0; i < uniforms; ++i)
  {
    glGetActiveUniform(m_program, (GLuint)i, longestUniform, &length, &size, &type, buffer.data());

    name.assign(buffer.data());
    //std::cout << "  [" << i << "]: " << name << "(" << m_uniforms[name] << ")" << std::endl;
    m_uniforms[name] = glGetUniformLocation(m_program, name.c_str());
  }
}

void Shader::scanAttributes()
{
  GLint attributes;
  GLint longestAttribute;

  GLint   size;
  GLenum  type;
  GLsizei length;
  std::string name;
  std::vector<char> buffer;

  glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &attributes);
  glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &longestAttribute);

  buffer.resize(longestAttribute);

  //std::cout << "Found : " << attributes << " attributes" << std::endl;
  for (GLint i = 0; i < attributes; ++i)
  {
    glGetActiveAttrib(m_program, (GLuint)i, longestAttribute, &length, &size, &type, buffer.data());

    name.assign(buffer.data());
    m_attributes[name] = glGetAttribLocation(m_program, name.c_str());
    //std::cout << "  [" << i << "]: " << name << "(" << m_attributes[name] << ")" << std::endl;
  }
}

void Shader::getProgramErrors()
{
  constexpr int SMALL_BUFFER_SIZE = 1024;

  GLint errorSize;
  char smallBuffer[SMALL_BUFFER_SIZE];
  std::vector<char> bigerBuffer;
  char *buffer;

  glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &errorSize);
  if (errorSize < SMALL_BUFFER_SIZE)
  {
    buffer = smallBuffer;
    buffer[errorSize] = 0;
  }
  else
  {
    bigerBuffer.resize(errorSize);
    buffer = bigerBuffer.data();
  }
  glGetProgramInfoLog(m_program, errorSize, nullptr, buffer);
  std::cout << "Shader linkage error: " << buffer << std::endl;
}

void Shader::getShaderErrors(GLuint shaderId)
{
  constexpr int SMALL_BUFFER_SIZE = 1024;

  GLint errorSize;
  char smallBuffer[SMALL_BUFFER_SIZE];
  std::vector<char> bigerBuffer;
  char *buffer;

  glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &errorSize);
  if (errorSize < SMALL_BUFFER_SIZE)
  {
    buffer = smallBuffer;
    buffer[errorSize] = 0;
  }
  else
  {
    bigerBuffer.resize(errorSize);
    buffer = bigerBuffer.data();
  }
  glGetShaderInfoLog(shaderId, errorSize, nullptr, buffer);
  std::cout << "Shader compile error: " << buffer << std::endl;
}

GLuint Shader::getOrCreate(Type type)
{
  GLuint result;

  if (!glIsShader(m_shaders[type]))
  {
    result = glCreateShader(typeToGl(type));
    m_shaders[type] = result;
  }
  else
    result = m_shaders.at(type);
  return result;
}