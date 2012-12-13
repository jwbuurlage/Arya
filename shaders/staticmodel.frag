#version 400

uniform sampler2D tex;

in vec2 texCoo;
in vec3 normal;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(tex, texCoo);
}
