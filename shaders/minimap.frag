#version 400
#extension GL_ARB_explicit_attrib_location : require

uniform sampler2D fogmap;
uniform sampler2D heightmap;

in vec2 texCoo;

layout(location=0) out vec4 color;

void main()
{
	color = vec4(0.5, 0.0, 0.0, 1.0); //vec4(texture(heightmap, texCoo).r * texture(fogmap, texCoo).r, 1.0);
}
