#version 400 
#
uniform vec2 screenSize;
uniform vec2 screenPosition;
uniform vec2 texOffset;
uniform vec2 texSize;

layout (location = 0) in vec2 position;

out vec2 texCoo;

void main()
{
    gl_Position = vec4(screenPosition + screenSize*position, 0.0, 1.0);
    texCoo = texOffset + texSize * position;
}
