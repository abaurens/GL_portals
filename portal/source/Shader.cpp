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
    result = glCreateShader(static_cast<GLenum>(type));
    m_shaders[type] = result;
  }
  else
    result = m_shaders.at(type);
  return result;
}