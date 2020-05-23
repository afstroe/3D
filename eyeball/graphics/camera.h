#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm.hpp> // vec3, vec4, ivec4, mat4
#include <gtc/matrix_transform.hpp> // translate, rotate, scale, perspective

#include <eyeball/utils/defines.h>

class Camera
{
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

  inline Camera():
    _mode(Mode::ORTHO)
  {

  }

  glm::mat4 transform();
};


#endif // !__CAMERA_H__
