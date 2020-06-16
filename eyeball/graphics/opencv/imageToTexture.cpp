#include "imageToTexture.h"
#include <Windows.h>
#include <gl/GL.h>
#include <eyeball/graphics/opengl/opengl_ext.h>
#include <eyeball/graphics/opengl/utils.h>
#include <eyeball/utils/paths.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/hal/interface.h>

int ImageToTexture::openGLTextureFromFile(const char* fileName)
{
  auto img = cv::imread(fileName, cv::IMREAD_UNCHANGED);
  unsigned char* datap = img.data;

  auto channelsNum = img.channels();

  GLenum format = 0; 
  GLenum internalFormat = 0;
  switch(channelsNum)
  {
    case 3:
      format = GL_RGB;
      internalFormat = GL_RGB8;
    break;
    case 2:
      format = GL_RG;
      internalFormat = GL_RG8;
    break;
    case 1:
      format = GL_R;
      internalFormat = GL_R8;
    break;
    case 4:
    default:
      format = GL_RGBA;
      internalFormat = GL_RGBA8;
  }

  if (channelsNum >= 3)// it seems we don't need to do it for four channels textures
  {
    auto convertTarget = channelsNum == 4 ? cv::COLOR_BGRA2RGBA : cv::COLOR_BGR2RGB;    
    cv::cvtColor(img, img, convertTarget);    
  }
  /// cv::flip(img, img, -1);

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
  

  float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
  // load and generate the texture
  int width = img.rows, height = img.cols;
  unsigned char* data = img.data;  

  if (data)
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  }

  if(1)
  {
    auto textureString = Paths::getFileName(fileName);
    ImageToTexture::dumpDebugOfTexture2D(texture, textureString);
  }

  OPENGL_CHECK_ERROR();
  
  return texture;
}

void ImageToTexture::dumpDebugOfTexture2D(int textureID, const char* outFileName, int mipLevel/* = 0*/)
{
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textureID);

  auto width = 0;
  auto height = 0;

  glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &height);

  cv::Mat img(width, height, CV_8UC(4));

  glGetTexImage(GL_TEXTURE_2D, mipLevel, GL_RGBA, GL_UNSIGNED_BYTE, img.data);

  auto* dumpPath = getenv("DEBUG_DUMP_PATH");
  std::string dumpFile = dumpPath;
  dumpFile += outFileName;
  dumpFile += ".png";

  cv::cvtColor(img, img, cv::COLOR_RGBA2BGRA);

  cv::imwrite(dumpFile.c_str(), img);

  glPopAttrib();
}
