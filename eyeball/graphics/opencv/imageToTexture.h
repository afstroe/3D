#ifndef __IMAGE_TO_TEXURE_H__
#define __IMAGE_TO_TEXURE_H__

class ImageToTexture
{
public:
  static int openGLTextureFromFile(const char* fileName);

  static void dumpDebugOfTexture2D(int textureID, const char* outFileName, int mipLevel = 0);
};
#endif // !__IMAGE_TO_TEXURE_H__
