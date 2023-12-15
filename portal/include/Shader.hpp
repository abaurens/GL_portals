#pragma once


#include <glad/glad.h>

#include <map>
#include <string>
#include <filesystem>

class Shader
{
public:
  enum class Type : GLenum
  {
    vertex = GL_VERTEX_SHADER,
    fragment = GL_FRAGMENT_SHADER,
    compute = GL_COMPUTE_SHADER,
    tesselation = GL_TESS_EVALUATION_SHADER,
    geometry = GL_GEOMETRY_SHADER,
  };

  ~Shader() { clear(); }

  bool addSource(Type shaderType, const std::string_view &source);
  bool addFile(Type shaderType, const std::filesystem::path &filePath);
  void remove(Type type);
  void clear();

  bool compile();

  GLuint id();
  GLuint id() const;

  bool valid() const { return m_valid; }

  GLint getUniform(const std::string &name) const;
  GLint getAttribute(const std::string &name) const;

private:
  void scanUniforms();
  void scanAttributes();
  void getProgramErrors();
  void getShaderErrors(GLuint shaderId);
  GLuint getOrCreate(Type shaderType);

  bool m_valid = false;
  GLuint m_program = 0;
  std::map<Type, GLuint> m_shaders;
  std::map<std::string, GLint> m_uniforms;
  std::map<std::string, GLint> m_attributes;
};