#include "shaders.h"

#include <fstream>
#include <gl/GL.h>
#include <unordered_map>

#include <eyeball/graphics/opengl/opengl_ext.h>
#include <eyeball/utils/debugout.h>
#include <eyeball/utils/defines.h>

namespace {
  bool readFile(const char* fileName,  std::string& out)
  {
    std::ifstream file;

    file.open(fileName, std::ifstream::in | std::ios::ate);
    if (file.good())
    {

      auto fileSize = static_cast<int>(file.tellg());

      file.seekg(0, file.beg);

      out.resize(fileSize);
      file.read(const_cast<char*>(out.c_str()), fileSize);
    }

    return file.eof();
  }

  bool compile(GLenum shaderType​, const std::string& shaderCode, GLuint& shader)
  {
    shader = glCreateShader(shaderType​);
    auto shaderCodeString = const_cast<char*>(shaderCode.c_str());
    glShaderSource(shader, 1, &shaderCodeString, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      GLint logSize = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
      std::string shaderLog;
      shaderLog.resize(logSize);
      auto shaderLogString = const_cast<char*>(shaderLog.c_str());
      GLsizei written = 0;
      glGetShaderInfoLog(shader, logSize, &written, shaderLogString);

      debugLog(">>> Shader failed to compile. Error log is:\n%", shaderLogString);
      glDeleteShader(shader);
      shader = 0;
    }

    return static_cast<bool>(success);
  }

  bool link(GLuint& vertexShader, GLuint& fragmentShader, GLuint& program)
  {
    program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

      std::string infoLog;
      infoLog.resize(maxLength);

      auto* infoLogString = const_cast<char*>(infoLog.c_str());

      glGetProgramInfoLog(program, maxLength, &maxLength, infoLogString);
      debugLog(">>> Shader failed to link. Error log is:\n%", infoLogString);

      glDeleteProgram(program);

      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);

      program = 0;
      vertexShader = 0;
      fragmentShader = 0;
    }
    else
    {
      glDetachShader(program, vertexShader);
      glDetachShader(program, fragmentShader);
      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);
      vertexShader = 0;
      fragmentShader = 0;
    }
    
    return program ? true : false;
  }

  std::unordered_map<std::string, int> extractUniforms(GLuint programID)
  {
    GLint numUniforms = 0;
    glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numUniforms);
    
    char uniformName[1024] = { 0 };
    GLenum type = GL_ZERO;
    int len = 0;
    int size = 0;
    
    std::unordered_map<std::string, int> uniforms;

    FOR(i, numUniforms)
    {      
      glGetActiveUniform(programID, i, 1023, &size, &len, &type, uniformName);
      uniformName[len] = 0;
      auto location = glGetUniformLocation(programID, uniformName);      
      uniforms[uniformName] = location;
    }

    return uniforms;
  }
}

Shader Shader::fromFiles(const char* vertexShader, const char* fragmentShader)
{
  std::string vertexShaderCode, fragmentShaderCode;

  GLuint glVertexShader = 0, glFragmentShader = 0, glProgram = 0;

  if (!readFile(vertexShader, vertexShaderCode))
  {
    debugLog(">>> Failed to read vertex shader %", vertexShader);
  }
  else
  {
    compile(GL_VERTEX_SHADER, vertexShaderCode, glVertexShader);
  }

  if (!readFile(fragmentShader, fragmentShaderCode))
  {
    debugLog(">>> Failed to read vertex shader %", fragmentShader);
  }
  else
  {
    compile(GL_FRAGMENT_SHADER, fragmentShaderCode, glFragmentShader);
  }

  if (vertexShader && fragmentShader)
  {
    if (!link(glVertexShader, glFragmentShader, glProgram))
    {
      return Shader();
    }
  }
  
  auto uniforms = extractUniforms(glProgram);
  return Shader(glProgram, uniforms);
}

Shader::~Shader()
{
  if (haveOpenGLContext())
  {
    if (programID > 0)
    {
      detach();
      glDeleteProgram(programID);
    }
  }
}
