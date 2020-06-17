#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <eyeball/utils/defines.h>

#include <vector>
#include <eyeball/graphics/opengl/opengl_ext.h>
#include <eyeball/graphics/opengl/material.h>
#include <eyeball/graphics/opengl/glm.h>
#include <eyeball/graphics/camera.h>
#include <eyeball/graphics/opengl/incOpenGL.h>


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
    Point3& operator += (const Point3& rhs)
    {
      x += rhs.x;
      y += rhs.y;
      z += rhs.z;

      return *this;
    }

    void set(Type x_, Type y_, Type z_)
    {
      this->x = x_;
      this->y = y_;
      this->z = z_;
    }

    void normalize() 
    {
      Type length_ = static_cast<Type>(1.0 / length());

      x *= length_;
      y *= length_;
      z *= length_;
    }
    Type length()
    {
      return static_cast<Type>(std::sqrt(x * x + y * y + z * z));
    }
  };

  template <typename Type>
  struct Point2
  {
    Type x;
    Type y;

    Point2()
    {
      x = 0;
      y = 0;
    }

    Point2(Type x, Type y)
    {
      this->x = x;
      this->y = y;
    }

    void set(Type x_, Type y_)
    {
      this->x = x_;
      this->y = y_;
    }

    Point2(const Point2& rhs)
    {
      x = rhs.x;
      y = rhs.y;
    }

    Type length()
    {
      return std::sqrt(x * x + y * y);
    }

    bool operator == (const Point2& rhs)
    {
      return rhs.x == x && rhs.y == y;
    }

    bool operator != (const Point2& rhs)
    {
      return !(*this == rhs);
    }
  };

  struct Vertex
  {
    Point3<float> position;
    Point3<float> normal;
    Point2<float> textureCoordinates;
    size_t index = 0;

    Vertex() = default;

    Vertex(const Vertex& rhs)
    {
      position = rhs.position;
      normal = rhs.normal;
      textureCoordinates = rhs.textureCoordinates;
      index = rhs.index;
    }

    struct Hash
    {
      std::size_t operator()(const Vertex& v) const noexcept
      {
        auto h1 = std::hash<float>{}(v.position.x);
        auto h2 = std::hash<float>{}(v.position.y);
        auto h3 = std::hash<float>{}(v.position.z);
        auto h4 = std::hash<float>{}(v.normal.x);
        auto h5 = std::hash<float>{}(v.normal.y);
        auto h6 = std::hash<float>{}(v.normal.z);
        auto h7 = std::hash<float>{}(v.textureCoordinates.x);
        auto h8 = std::hash<float>{}(v.textureCoordinates.y);

        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6) ^ (h8 << 7);
      }
    };

    bool operator == (const Vertex& rhs) const
    {
      static const float eps = 0.00001f;
      if (std::fabs(position.x - rhs.position.x) < eps &&
        std::fabs(position.y - rhs.position.y) < eps &&
        std::fabs(position.z - rhs.position.z) < eps &&
        std::fabs(normal.x - rhs.normal.x) < eps &&
        std::fabs(normal.y - rhs.normal.y) < eps &&
        std::fabs(normal.z - rhs.normal.z) < eps &&
        std::fabs(textureCoordinates.x - rhs.textureCoordinates.x) < eps &&
        std::fabs(textureCoordinates.y - rhs.textureCoordinates.y) < eps)
      {
        return true;
      }
      else
      {
        return false;
      }
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
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::vector<Point2<float>>, textureCoordinates);

  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::vector<int>, indices);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(size_t, numTriangles);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(Mode, mode);
  DECLARE_SHARED_POINTER(protected, Material, material);

  GLuint vertexBuffer = 0;
  GLuint normalBuffer = 0;
  GLuint textureCoordinatesBuffer = 0;
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

protected:
  void drawNormals(const Camera& camera);

};


#endif // !__GEOMETRY_H__
