#version 330 core
#define GLSLIFY 1

// uniforms
uniform int wireframe;

uniform int drawMonochrome;

uniform struct Material
{  
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emissive;
  float shininess;
  // Emissive = 0, Ambient = 1, Diffuse = 2, Specular = 3
  sampler2D textures[4];
  int texturesPresent;
}material;

uniform vec3 cameraPosition;

// varying
in vec3 normalToFragment;
in vec3 positionToFragment;
in vec2 textureCoordinatesToFragment;

// output
layout (location = 0) out vec4 color;