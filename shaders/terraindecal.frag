#version 400
#extension GL_ARB_explicit_attrib_location : require

uniform sampler2D selectionTexture;
uniform vec3 uColor;

layout (location = 0) out vec4 fragColor;

in vec2 texcoo;

void main()
{
    fragColor = vec4(uColor, 1.0) * texture(selectionTexture, texcoo);
}
