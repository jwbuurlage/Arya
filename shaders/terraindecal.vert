#version 400
#extension GL_ARB_explicit_attrib_location : require

uniform mat4 vpMatrix;
uniform vec2 oneOverTerrainSize;

uniform sampler2D heightMap;
layout (location = 0) in vec2 posIn;
out vec2 texcoo;

uniform float scaleFactor;
uniform vec2 offset;

float height(vec2 tco)
{
    vec4 h = vec4(0.0);
    h = texture(heightMap, tco);
    return h.r;
}

void main()
{
	texcoo = posIn;
	vec2 position = posIn;
	
	position -= vec2(0.5);
	position *= scaleFactor;
	position += offset;
	
	gl_Position = vpMatrix * vec4(
		position.x,
		150.0 * height( vec2(position.x * oneOverTerrainSize.x, position.y * oneOverTerrainSize.y) + vec2(0.5) ),
		position.y,
		1.0 );
}
