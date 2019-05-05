#version 400
in vec3 vertColour;
out vec4 outColour;
void main ()
{
    // set the fragment colour
    outColour = vec4(vertColour,1);
}
