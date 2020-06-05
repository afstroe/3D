void main()
{
  normalToFragment = (modelMatrix * vec4(normal, 1.0)).xyz;
  positionToFragment = (modelMatrix * vec4(position, 1.0)).xyz;
  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}