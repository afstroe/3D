void main()
{
  normalToFragment = (modelMatrix * vec4(normal, 1.0)).xyz;
  // normalToFragment = normal;
  positionToFragment = (modelMatrix * vec4(position, 1.0)).xyz;

  textureCoordinatesToFragment = texCoords0;

  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}