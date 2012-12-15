#version 400

uniform sampler2D selectionTexture;

layout (location = 0) out vec4 fragColor;

in vec2 texcoo;

void main()
{
    fragColor = texture(selectionTexture, texcoo);
}
