#version 140
#extension GL_ARB_explicit_attrib_location : require

in vec2 texcoo;

uniform sampler2D decalTexture;
uniform vec3 decalColor;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = vec4(decalColor, 1.0) * texture(decalTexture, texcoo);
}
