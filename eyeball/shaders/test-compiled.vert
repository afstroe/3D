#version 330 core
#define GLSLIFY 1

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords0;

uniform int a;

out float fa;

void main()
{
  // TODO: replace the compatibility matrices with 
  // uniform buffers
  // vec4 p = gl_ModelViewMatrix * vec4(position, 1.0); 
  // gl_Position = gl_ProjectionMatrix * p;
  gl_Position = vec4(position, 1.0);
  fa = a / 255.0;
}