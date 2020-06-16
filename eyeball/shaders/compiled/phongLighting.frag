#define GLSLIFY 1
vec4 phongLighting(Material material)
{
  vec3 light = vec3(0, -2, -30);

  vec3 lightColor = vec3(1, 1, 1);
  vec3 position = positionToFragment;
  vec3 diffuseColor = material.diffuse.rgb;
  vec3 normal = normalToFragment;

  // normalize the incoming n, l anf v vectors
  vec3 n = normalize(normal);
  vec3 l = normalize(-light);
  vec3 v = normalize(position.xyz - light);

  // the reflection of the light source into the rendered surface
  vec3 r = reflect(normalize(cameraPosition - light), n);

  // color from textures
  // Emissive = 0, Ambient = 1, Diffuse = 2, Specular = 3
  vec4 ambientTexture = ((material.texturesPresent & 2) != 0) ? texture(material.textures[1], textureCoordinatesToFragment.st) : vec4(1);
  vec4 diffuseTexture = ((material.texturesPresent & 4) != 0) ? texture(material.textures[2], textureCoordinatesToFragment.st) : vec4(1);
  vec4 specularTexture = ((material.texturesPresent & 8)  != 0) ? texture(material.textures[3], textureCoordinatesToFragment.st) : vec4(1);

  // the diffuse and specular components for each fragment
  vec3 ambient = lightColor * ambientTexture.rgb;
  vec3 diffuse = ((diffuseColor.rgb * min( max(dot(n, l), 0), 1.0 ))) * diffuseTexture.rgb;
  vec3 specular = pow(max(dot(r, v), 0.0), 1000.0) * lightColor * specularTexture.rgb;

  // if(length(diffuseTexture.rgb) < 0.2)
  // {
  //   return vec4(1.0, 0.0, 0.0, 1.0);  
  // }

  // return vec4(diffuseTexture.rgb, 1.0);

  float alpha = (ambientTexture.a + diffuseTexture.a + specularTexture.a) * 0.33;
  
  // return vec4(abs(normal.x), abs(normal.y), abs(normal.z), 1.0);
  // return vec4(textureCoordinatesToFragment.st, 1, 1);

  return vec4(2 * (ambient * diffuse) /*+ specular*/, alpha);
}

