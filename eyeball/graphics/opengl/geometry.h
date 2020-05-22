#ifndef __GEOMETRY_H__

#include <eyeball/utils/defines.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <gl/GL.h>
#include <vector>
#include <eyeball/graphics/opengl/opengl_ext.h>

class Geometry
{
public:
  template <typename Type>
  struct Point2 {
    Type x;
    Type y;
    Point2()
    {
      x = 0;
      y = 0;
    }
  };

  struct Mode
  {
    GLuint drawMode = GL_TRIANGLES;
    GLuint fillMode = GL_FILL;
    GLuint faceMode = GL_FRONT;
  };

  enum DebugMode 
  {
    DM_DISABLED = 0,
    DM_WIREFRAME = 1 << 0, 
    DM_WIRE_SOLID = 1 << 1,
  };

  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::vector<Point2<float>>, vertices);

  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::vector<int>, indices);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(size_t, numTriangles);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(Mode, mode);

  GLuint vertexBuffer = 0;
  GLuint indexBuffer = 0;


  Geometry() :
    _numTriangles(0)
  {
    ;
  }

  inline void bind()
  {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  }

  void createBuffers();

  void draw(bool wireSolid = false);

  void drawImmediate();
};


#endif // !__GEOMETRY_H__
