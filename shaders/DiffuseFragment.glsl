#version 400
in vec3 fragNormal;
in vec3 eyeVector;

uniform vec4 color;

out vec4 fragColor;

void main(void)
{
  fragColor=color*dot(fragNormal,eyeVector);
}



