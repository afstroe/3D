#define GLSLIFY 1
vec4 phongLighting(Material material)
{
  vec3 light = vec3(0, 10, 0);

  vec3 lightColor = vec3(1, 1, 1);
  vec3 position = positionToFragment;
  vec3 diffuseColor = material.diffuse.rgb;
  vec3 normal = normalToFragment;

  // normalize the incoming n, l anf v vectors
  vec3 n = normalize(normal);
  vec3 l = normalize(light);
  vec3 v = normalize(light - position.xyz);

  // the reflection of the light source into the rendered surface
  vec3 r = reflect(normalize(light - cameraPosition), n);

  // the diffuse and specular components for each fragment
  vec3 ambient = lightColor;
  vec3 diffuse = diffuseColor.rgb * max(dot(n, l), 0.0);
  vec3 specular = pow(max(dot(r, v), 0.0), 10) * lightColor;

  return vec4(diffuse, 1.0);

  // return vec4((ambient * diffuse + specular), 1.0);  
}

