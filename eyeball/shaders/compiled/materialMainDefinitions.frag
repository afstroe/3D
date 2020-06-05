#version 330 core
#define GLSLIFY 1

// uniforms
uniform int wireframe;

uniform struct Material
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emissive;
  float shininess;
}material;

uniform vec3 cameraPosition;

// varying
in vec3 normalToFragment;
in vec3 positionToFragment;

// output
layout (location = 0) out vec4 color;