#include "material.h"
#include <eyeball/utils/paths.h>
#include <eyeball/graphics/opencv/imageToTexture.h>
#include <eyeball/graphics/opengl/glm.h>


namespace {
  std::string textureNameString(Material::TextureNames textureName)
  {
    switch (textureName)
    {
      case Material::TextureNames::NoName:
        return "NoName";
      case Material::TextureNames::Emissive:
        return "Emissive";
      case Material::TextureNames::Ambient:
        return "Ambient";
      case Material::TextureNames::Diffuse:
        return "Diffuse";
      case Material::TextureNames::Specular:
        return "Specular";
      
      default:
        return "";
      break;
    }
  }
}

void Material::loadTextures()
{
  for (TextureNames textureName = TextureNames::Emissive; textureName < TextureNames::Num; ++textureName)
  {
    const auto fileName = textureFile(textureName);
    if (fileName != "")
    {
      texture(textureName) = ImageToTexture::openGLTextureFromFile(fileName.c_str());
      m_texturesFlag |= (1 << (textureName - 1));
    }
  }
}

void Material::preDraw()
{
  m_shader->set("material.ambient", glUniform4fv, 1, glm::value_ptr(m_ambient));
  m_shader->set("material.diffuse", glUniform4fv, 1, glm::value_ptr(m_diffuse));
  m_shader->set("material.specular", glUniform4fv, 1, glm::value_ptr(m_specular));
  m_shader->set("material.emissive", glUniform4fv, 1, glm::value_ptr(m_emissive));
  m_shader->set("material.shininess", glUniform1f, m_shininess);  
  m_shader->set("material.texturesPresent", glUniform1i, m_texturesFlag);
  
  for (TextureNames textureName = TextureNames::Emissive; textureName < TextureNames::Num; ++textureName)
  {
    if (GLuint tex = texture(textureName))
    {
      std::stringstream texvar;
      texvar << "material.textures[" << textureName - 1 << "]";
      glActiveTexture(GL_TEXTURE0 + textureName - 1);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex);

      if(0)
      { 
        auto textureString = textureNameString(textureName);
        textureString = m_name + "_" + textureString;
        ImageToTexture::dumpDebugOfTexture2D(tex, textureString.c_str());
      }

      m_shader->set(texvar.str().c_str(), glUniform1i, textureName - 1);
    }
  }
}

void Material::postDraw()
{
  for (TextureNames textureName = TextureNames::Emissive; textureName < TextureNames::Num; ++textureName)
  {
    glActiveTexture(GL_TEXTURE0 + textureName - 1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
  }
}
