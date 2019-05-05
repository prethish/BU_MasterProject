#version 400
//calculates the color based on camerra position

in vec3 inVert;
in vec2 inUV;
in vec3 inNormal;

uniform vec3 camPos;
uniform vec4 color;
uniform mat4 M;
uniform mat4 MVP;

out vec3 fragNormal;
out vec3 eyeVector;
out vec2 vertUV;

void main(void)
{
//vertex position
  gl_Position = MVP*vec4(inVert, 1.0);
//fragment normal calculation
  fragNormal=normalize(inNormal);
//eye vector calculation
  vec4 pointWorldSpace=M*vec4(inVert,1);
  eyeVector=normalize(camPos-pointWorldSpace.xyz);

  vertUV=inUV.st;

}
