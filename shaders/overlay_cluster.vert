#version 140
#extension GL_ARB_explicit_attrib_location : require

layout(location=0) in vec2 position;
layout(location=1) in vec2 texIn;

uniform mat4 pixelToScreenTransform;
uniform vec2 screenOffset;

out vec2 texCoo;

void main()
{
    gl_Position = vec4(screenOffset, 0.0, 0.0) + pixelToScreenTransform * vec4(position, 0.0, 1.0);
    texCoo = texIn;
}
