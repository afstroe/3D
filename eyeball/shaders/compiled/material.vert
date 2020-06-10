#version 330 core
#define GLSLIFY 1
#define GLSLIFY 1

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords0;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;

out vec3 normalToFragment;
out vec3 positionToFragment;
out vec2 textureCoordinatesToFragment;
#define GLSLIFY 1
void main()
{
  normalToFragment = (modelMatrix * vec4(normal, 1.0)).xyz;
  // normalToFragment = normal;
  positionToFragment = (modelMatrix * vec4(position, 1.0)).xyz;

  textureCoordinatesToFragment = texCoords0;

  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}