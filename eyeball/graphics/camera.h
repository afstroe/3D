#ifndef __CAMERA_H__
#define __CAMERA_H__

#pragma warning (push, 4)
#pragma warning (disable:4201)
#pragma warning (disable:4127)
#define GLM_FORCE_SWIZZLE
#include <glm.hpp> // vec3, vec4, ivec4, mat4
#include <gtc/matrix_transform.hpp> // translate, rotate, scale, perspective
#include <gtc/type_ptr.hpp>
#pragma warning (pop)

#include <eyeball/utils/defines.h>

class Camera
{
public:
  enum class Mode
  {
    ORTHO = 0,
    PERSPECTIVE
  };

  struct Perspective
  {
    float verticalAngle = 45.0f;    // degrees
    float aspectRatio = 4.0f / 3.0f;
    float nearPlane = 0.1f;          // meters
    float farPlane = 1000;          // meters
  };

  struct Ortho
  {
    float left = -0.5;
    float right = 0.5;
    float bottom = -0.5;
    float top = 0.5;
    float zNear = -1000;
    float zFar = 1000;
  };  

  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec3, position);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec3, attitude);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(Mode, mode);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(Perspective, perspectiveData);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(Ortho, parallelData);

  inline const float& heading() const
  {
    return m_attitude.y;
  }
  inline float& heading()
  {
    return m_attitude.y;
  }

  inline const float& pitch() const
  {
    return m_attitude.x;
  }
  inline float& pitch()
  {
    return m_attitude.x;
  }

  inline const float& roll() const
  {
    return m_attitude.z;
  }
  inline float& roll()
  {
    return m_attitude.z;
  }

  inline Camera():
    m_mode(Mode::ORTHO)
  {

  }

  glm::mat4 transform();
  glm::mat4 projectionMatrix();
  glm::mat4 mvMatrix();
  glm::mat4 attitudeMatrix();  
  glm::mat4 lookAt(const glm::vec3& at);

};


#endif // !__CAMERA_H__
