#version #version 400

uniform sampler2D texture1;
in vec2 texCoo;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(texture1, texCoo);
}
