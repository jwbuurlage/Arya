#version 400
#extension GL_ARB_explicit_attrib_location : require

layout(location=0) in vec2 position;
layout(location=1) in vec2 texCooIn;

out vec2 texCoo;

void main()
{
	texCoo = texCooIn;
	gl_Position = vec4(position, 0.0, 1.0);
}
