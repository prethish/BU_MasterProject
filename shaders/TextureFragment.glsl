#version 400
in vec3 fragNormal;
in vec3 eyeVector;
in vec2 vertUV;

uniform vec4 color;
uniform sampler2D tex;
out vec4 fragColor;

void main(void)
{
  fragColor=texture(tex,vertUV)*dot(fragNormal,eyeVector);
}



