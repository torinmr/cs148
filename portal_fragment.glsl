#version 330 core

in vec3 Normal;

out vec4 color;

uniform int type;
uniform float time;

void main()
{
  float intensitya = sin(time*3)*0.25 + 0.75;
  float intensityb = cos(time*3)*0.25 + 0.75;
  if (type == 0) {
    color = vec4(intensitya, 0.0f, intensityb, 1.0f);
  } else if (type == 1) {
    color = vec4(0.0f, intensityb, intensitya, 1.0f);
  }
}