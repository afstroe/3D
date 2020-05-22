#ifndef __OPENGL_UTILS_H__
#define __OPENGL_UTILS_H__

#include <eyeball/utils/debugout.h>
#include <gl/GLU.h>
#include <string.h>

#pragma comment (lib, "glu32.lib")

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


#endif // !__OPENGL_UTILS_H__

