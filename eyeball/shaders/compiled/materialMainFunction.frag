#define GLSLIFY 1
vec4 wireframeColor()
{
  return vec4(vec3(0.3), 1.0);
}

vec4 phongLighting(Material material)
{
  vec3 ambient = /*lightColor*/ material.ambient * 0.1;

  return vec4(1.0, 0.0, 0.0, 1.0);
}

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