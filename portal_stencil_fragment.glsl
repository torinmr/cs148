#version 330 core

out vec4 color;

uniform vec4 backgroundColor;

void main()
{
  color = backgroundColor;
}