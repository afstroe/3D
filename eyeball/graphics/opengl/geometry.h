#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <eyeball/utils/defines.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <gl/GL.h>
#include <vector>
#include <eyeball/graphics/opengl/opengl_ext.h>
#include <eyeball/graphics/opengl/material.h>
#include <eyeball/graphics/opengl/glm.h>
#include <eyeball/graphics/camera.h>


class Geometry
{
public:
  template <typename Type>
  struct Point3 {
    Type x;
    Type y;
    Type z;
    Point3()
    {
      x = 0;
      y = 0;
      z = 0;
    }
    Point3(Type x, Type y, Type z)
    {
      this->x = x;
      this->y = y;
      this->z = z;
    }
  };

  struct Mode
  {
    GLuint drawMode = GL_TRIANGLES;
    GLuint fillMode = GL_FILL;
    GLuint faceMode = GL_FRONT;
  };

  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::vector<Point3<float>>, vertices);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::vector<Point3<float>>, normals);

  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::vector<int>, indices);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(size_t, numTriangles);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(Mode, mode);
  DECLARE_SHARED_POINTER(protected, Material, material);

  GLuint vertexBuffer = 0;
  GLuint normalBuffer = 0;
  GLuint indexBuffer = 0;


  Geometry() :
    m_numTriangles(0)
  {
    ;
  }

  inline void bind()
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);    

    m_material->shader()->attach();
  }

  void createBuffers();

  void preDraw();

  void draw(const Camera& camera);

  void postDraw();

  void drawImmediate();  

  ~Geometry();
};

class PositionedGeometry : public Geometry
{
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::mat4, transform);
public:
  PositionedGeometry() :
    Geometry()
  , m_transform(glm::mat4(1))
  {
    ;
  }

  void draw(const Camera& camera);
};


#endif // !__GEOMETRY_H__
