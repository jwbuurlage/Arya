#version 400
#extension GL_ARB_explicit_attrib_location : require

layout(location=0) in vec2 position;

void main()
{
	gl_Position = vec4(position, 0.0, 1.0);
}
