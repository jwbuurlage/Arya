#version 140
#extension GL_ARB_explicit_attrib_location : require

layout (location = 0) in vec3 vertexPosition;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;
out vec2 texCoo;

void main()
{
    texCoo = vertexPosition.xy;
    gl_Position = vpMatrix * mMatrix * vec4(vertexPosition, 1.0);
}
