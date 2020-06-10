#ifndef __OPENGL_UTILS_H__
#define __OPENGL_UTILS_H__

#include <eyeball/utils/debugout.h>
#include <gl/GLU.h>
#include <string.h>

#pragma comment (lib, "glu32.lib")

#ifdef ENABLE_OPENGL_ERROR_CHECKING
#define OPENGL_CHECK_ERROR()                                                                                                           \
    {                                                                                                                                  \
      if(const GLenum openglError = glGetError())                                                                                      \
      {                                                                                                                                \
        if(!IsDebuggerPresent())                                                                                                       \
        {                                                                                                                              \
          debugLog("OpenGL error % found on file '%', line %",    _strupr((char*)gluErrorString(openglError)), __FILE__, __LINE__);    \
        }                                                                                                                              \
        else                                                                                                                           \
        {                                                                                                                              \
          const GLubyte* openglErrorString = gluErrorString(openglError);                                                              \
          UNREFERENCED_PARAMETER(openglErrorString);                                                                                   \
          __debugbreak();                                                                                                              \
        }                                                                                                                              \
      }                                                                                                                                \
    }
#define OPENGL_CHECK_CONTEXT()                                                            \
{                                                                                         \
    if(wglGetCurrentContext() == NULL)                                                    \
    {                                                                                     \
        if(!IsDebuggerPresent())                                                          \
        {                                                                                 \
          debugLog("No OpenGL context found on file '%', line %", __FILE__, __LINE__);    \
        }                                                                                 \
        else                                                                              \
        {                                                                                 \
            __debugbreak();                                                               \
        }                                                                                 \
    }                                                                                     \
}
#else
#define OPENGL_CHECK_ERROR()
#define OPENGL_CHECK_CONTEXT()
#endif


inline bool haveOpenGLContext()
{
  return wglGetCurrentContext() != NULL;
}

inline void resetOpenGLError()
{
  glGetError();
}

#endif // !__OPENGL_UTILS_H__

