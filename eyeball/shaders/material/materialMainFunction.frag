
#import wireframeColor from ../utils/utils.frag
#import monochromeColor from ../utils/utils.frag
#import phongLighting from ./phongLighting.frag

void main()
{
  if(wireframe == 1)
  {
    color = wireframeColor();
  }
  else if(drawMonochrome == 1)
  {
    color = monochromeColor();
  }
  else
  {
    color = phongLighting(material);
  }
}