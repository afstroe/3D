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
  static Shader fromFiles(const char* vertexShader, const char* fragmentShader);

  ~Shader();

  void attach() 
  {
    unsigned int activeProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&activeProgram);
    if (activeProgram != programID)
    {
      glUseProgram(programID);
    }    
  }

  static void detach()
  {
    glUseProgram(0);
  }

  Shader() :
    programID(0)
  {
    ;
  }

  Shader& operator = (const Shader& rhs) 
  {
    this->programID = rhs.programID;
    const_cast<Shader&>(rhs).programID = 0;// otherwise will get deleted by the destructor
    this->uniforms = std::move(rhs.uniforms);
    return *this;
  }

  template<typename FunctionType, typename... Args>
  void set(const char* uniform, FunctionType function, Args... args)
  {
    attach();    
    function(uniforms[uniform], args...);
    OPENGL_CHECK_ERROR();
  }

protected:
  Shader(GLuint _programID, std::unordered_map<std::string, int>& _uniforms) :
    programID(_programID)  
  {
    uniforms = std::move(_uniforms);
  }  

protected:
  GLuint programID;
  std::unordered_map<std::string, int> uniforms;
};

#endif // !__SHADERS_H__
