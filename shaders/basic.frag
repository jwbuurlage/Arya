#version 140

in vec3 Color;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = vec4(Color, 1.0);
}
