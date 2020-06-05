#ifndef __SHADERS_H__
#define __SHADERS_H__

#include <Windows.h>
#include <gl/GL.h>
#include <eyeball/graphics/opengl/opengl_ext.h>
#include <eyeball/graphics/opengl/utils.h>
#include <unordered_map>

class Shader
{
public:
  static std::unique_ptr<Shader> fromFiles(const char* vertexShader, const char* fragmentShader);

  ~Shader();

  void attach() 
  {
    unsigned int activeProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&activeProgram);
    if (activeProgram != m_programID)
    {
      glUseProgram(m_programID);      
    }
  }

  static void detach()
  {    
    glUseProgram(0);
  }

  Shader() :
    m_programID(0)
  {
    ;
  }

  Shader& operator = (const Shader& rhs) 
  {
    this->m_programID = rhs.m_programID;
    const_cast<Shader&>(rhs).m_programID = 0;// otherwise will get deleted by the destructor
    this->m_uniforms = std::move(const_cast<Shader&>(rhs).m_uniforms);
    return *this;
  }

  template<typename FunctionType, typename... Args>
  void set(const char* uniform, FunctionType function, Args... args)
  {
    attach();
    auto uniformite = m_uniforms.find(uniform);
    int location = -1;
    if (uniformite == m_uniforms.end())
    {
      location = glGetUniformLocation(m_programID, uniform);      
      m_uniforms[uniform] = location;

      if (location == -1)
      {
        debugLog("(!) Uniform % not found in the shader", uniform);
      }
    }
    else
    {
      location = (*uniformite).second;
    }

    function(location, args...);    
    OPENGL_CHECK_ERROR();
  }

  Shader(GLuint programID, std::unordered_map<std::string, int>& uniforms) :
    m_programID(programID)  
  {
    m_uniforms = std::move(uniforms);
  }

  void construct(const char* vertexShader, const char* fragmentShader);

protected:
  GLuint m_programID;
  std::unordered_map<std::string, int> m_uniforms;
};

#endif // !__SHADERS_H__
