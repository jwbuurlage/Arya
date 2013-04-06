#version 140
#extension GL_ARB_explicit_attrib_location : require

in vec2 texCoo;
layout(location=0) out vec4 fragColor;
uniform sampler2D texture1;

void main()
{
    fragColor = texture(texture1, texCoo).r * vec4(1.0) + vec4(0.2);
}
