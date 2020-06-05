#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "shaders.h"
#include "glm.h"
#include "eyeball/utils/defines.h"

class Material
{
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec4, emissive);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec4, ambient);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec4, diffuse);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec4, specular);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(float, shininess);

  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::string, emissiveTexture);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::string, ambientTexture);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::string, diffuseTexture);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::string, specularTexture);

  DECLARE_PROTECTED_TRIVIAL_PTR_ATTRIBUTE(Shader, shader);

public:
  Material():
    m_shininess(0.0f)
  {
    ;
  }
};

#endif // #ifndef __MATERIAL_H__