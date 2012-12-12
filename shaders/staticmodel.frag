#version 400

uniform sampler2D tex;

in vec2 texCoo;
in vec2 normal;

layout (location = 0) vec4 fragColor;

void main()
{
    fragColor = texture(tex, texCoo);
}
