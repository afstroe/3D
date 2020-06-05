
#import wireframeColor from ../utils/utils.frag
#import phongLighting from ./phongLighting.frag

void main()
{
  if(wireframe == 1)
  {
    color = wireframeColor();
  }
  else
  {
    color = phongLighting(material);
  }
}