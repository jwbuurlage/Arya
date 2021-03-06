#version 400
#extension GL_ARB_explicit_attrib_location : require

uniform sampler2D texture1;
uniform vec4 uColor;
in vec2 texCoo;
layout(location=0) out vec4 fragColor;

vec4 overlayColor(vec2 tex)
{
    return texture(texture1,texCoo);
}


void main()
{
    fragColor = uColor * overlayColor(texCoo);
}
