#version 400

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;
out vec3 Color;

void main()
{
    Color = vertexColor;
    gl_Position = vpMatrix * mMatrix * vec4(vertexPosition, 1.0);
}
