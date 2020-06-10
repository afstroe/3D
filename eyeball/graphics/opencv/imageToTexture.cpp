#include "imageToTexture.h"
#include <Windows.h>
#include <gl/GL.h>
#include <eyeball/graphics/opengl/opengl_ext.h>
#include <eyeball/graphics/opengl/utils.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

int ImageToTexture::openGLTextureFromFile(const char* fileName)
{
  auto img = cv::imread(fileName, cv::IMREAD_UNCHANGED);

  auto channelsNum = img.channels();

  GLenum format = 0; 
  GLenum internalFormat = 0;
  switch(channelsNum)
  {
    case 3:
      format = GL_RGB;
      internalFormat = GL_RGB;
    break;
    case 2:
      format = GL_RG;
      internalFormat = GL_RG;
    break;
    case 1:
      format = GL_R;
      internalFormat = GL_R;
    break;
    case 4:
    default:
      format = GL_RGBA;
      internalFormat = GL_RGBA;
  }

  if (channelsNum >= 3)
  {
    auto convertTarget = channelsNum == 4 ? cv::COLOR_BGRA2RGBA : cv::COLOR_BGR2RGB;    
    cv::cvtColor(img, img, convertTarget);
    cv::flip(img, img, -1);
  }

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width = img.rows, height = img.cols;
  unsigned char* data = img.data;

  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);
  }
  
  return texture;
}
