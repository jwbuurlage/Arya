#version 140

uniform mat4 vpMatrix;
uniform mat4 scaleMatrix;

uniform vec3 groundPosition;
uniform float unitRadius;
uniform float yOffset;

out vec2 texcoo;

layout (location = 0) in vec2 position;

void main()
{
    vec4 pos = vec4(groundPosition.x + unitRadius * (position.x - 0.5f),
                        groundPosition.y + yOffset,
                        groundPosition.z + unitRadius * (position.y - 0.5f),
                        1.0);

    texcoo = position;
    gl_Position = vpMatrix * pos;
}
