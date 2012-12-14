#version 400

uniform sampler2D heightMap;

uniform mat4 vpMatrix;
uniform mat4 scaleMatrix;

uniform vec2 groundPosition;

out vec2 texcoo;

layout (location = 0) in vec2 position;

void main()
{
    texcoo = position;
    gl_Position = vec4(position.x, 130.0, position.y, 1.0);
}
