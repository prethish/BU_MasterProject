#version 400
/// @brief Model View Projection Matrix
uniform mat4 MVP;
///// @brief the vertex passed in
in vec3 inVert;
///// @brief the vertex color passed in
uniform vec3 color;
out vec3 vertColour;
void main(void)
{
    // calculate the vertex position
    gl_Position = MVP*vec4(inVert, 1.0);
    vertColour=color;
}
