#version 140

layout (location = 0) in vec3 vertexPosition;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;
out vec3 Color;

void main()
{
    Color = vertexPosition;
    gl_Position = vpMatrix * mMatrix * vec4(vertexPosition, 1.0);
}
