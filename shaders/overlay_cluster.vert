#version 400
#extension GL_ARB_explicit_attrib_location : require

layout(location=0) in vec2 position;
layout(location=1) in vec2 texIn;

uniform mat4 pixelToScreenTransform;

out vec2 texCoo;

void main()
{
    gl_Position = pixelToScreenTransform * vec4(position, 0.0, 1.0);
    texCoo = texIn;
}
