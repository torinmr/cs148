#version 330 core

in vec3 Normal;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main()
{
  color = vec4(1.0f, 0.0f, 1.0f, 1.0);
}