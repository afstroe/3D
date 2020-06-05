#ifndef __FBX_BUILDER_H__
#define __FBX_BUILDER_H__

#include <fbxsdk.h>
#include <eyeball/graphics/opengl/geometry.h>

class FbxBuilder
{
public:
  void load(const char* fileName, std::vector<PositionedGeometry*>& fbxGeometry);
protected:
  void parseScene(FbxScene* scene, std::vector<PositionedGeometry*>& fbxGeometry);
  ;
};

#endif // !__FBX_BUILDER_H__
