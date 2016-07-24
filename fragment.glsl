#version 330 core

in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 lightDir;
uniform sampler2D texture_diffuse1;

void main()
{
  vec3 diffuseTexel = texture(texture_diffuse1, TexCoords).xyz;
  vec3 normal = normalize(Normal);

  vec3 ambientColor = diffuseTexel;

  float diffuseFactor = max(-1*dot(lightDir, normal), 0);
  vec3 diffuseColor = diffuseFactor * diffuseTexel;

  color = vec4(ambientColor*.3 + diffuseColor*.7, 1.0);
}