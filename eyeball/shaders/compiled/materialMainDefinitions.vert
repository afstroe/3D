#version 330 core
#define GLSLIFY 1

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords0;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;

out vec3 normalToFragment;
out vec3 positionToFragment;