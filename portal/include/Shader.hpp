#pragma once


#include <glad/gl.h>

#include <map>
#include <string>
#include <filesystem>

class Shader
{
public:
  enum class Type : uint8_t
  {
    none        = 0,
    vertex      = 0b00001,
    fragment    = 0b00010,
    compute     = 0b00100,
    geometry    = 0b01000,
    tesselation = 0b10000,
  };

  Shader() noexcept;
  Shader(Shader &&mv) noexcept;
  ~Shader() { clear(); }

  Shader &operator=(Shader &&other) noexcept;

  bool addSource(Type type, const std::vector<std::string_view> &source);
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

  static const Shader &getDefaultShader();
  static Type typeFromString(const std::string_view name);

private:
  static GLenum typeToGl(Type type);

  static Shader default_shader;

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

static inline bool operator!(Shader::Type a)
{
  return !static_cast<uint8_t>(a);
}

static inline Shader::Type operator&(Shader::Type a, Shader::Type b)
{
  return static_cast<Shader::Type>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

static inline Shader::Type operator|(Shader::Type a, Shader::Type b)
{
  return static_cast<Shader::Type>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

static inline Shader::Type &operator|=(Shader::Type &a, Shader::Type b)
{
  a = static_cast<Shader::Type>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
  return a;
}
