#version 330 core
#define GLSLIFY 1

in float fa;

// output
layout (location = 0) out vec4 color;

void main()
{
  color = vec4(1.0, fa, 0.0, 1.0);
}