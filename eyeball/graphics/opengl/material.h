#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "shaders.h"
#include "glm.h"
#include "eyeball/utils/defines.h"
#include "eyeball/utils/enum.h"

class Material
{
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec4, emissive);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec4, ambient);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec4, diffuse);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(glm::vec4, specular);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(float, shininess);
  DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(std::string, baseFolder);

  DECLARE_PROTECTED_TRIVIAL_PTR_ATTRIBUTE(Shader, shader);

  Enum(TextureNames, NoName, Emissive, Ambient, Diffuse, Specular, Num);

  inline GLuint& texture(TextureNames textureName)
  {
    return m_textures[textureName];
  }

  inline const char* textureFile(TextureNames textureName) const
  {
    return m_textureFiles[textureName].c_str();
  }

  inline std::string& textureFile(TextureNames textureName)
  {
    return m_textureFiles[textureName];
  }

public:
  Material() :
    m_shininess(0.0f)
  {
    ;
  }

  void loadTextures();

  void preDraw();
  
  void postDraw();

protected:
  GLuint m_textures[TextureNames::Num] = { 0 };
  std::string m_textureFiles[TextureNames::Num];
  unsigned long m_texturesFlag = 0;
};

#endif // #ifndef __MATERIAL_H__