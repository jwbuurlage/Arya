#version 140

uniform sampler2D heightMap;

uniform mat4 vpMatrix;
uniform mat4 scaleMatrix;

uniform vec2 groundPosition;

out vec2 texcoo;

layout (location = 0) in vec2 position;

float height(vec2 tco)
{
    vec4 h = vec4(0.0);
    h = texture(heightMap, tco);
    return h.r;
}

void main()
{
    vec4 pos = vec4(groundPosition.x + 10.0*(position.x - 0.5), 
                        0.0,
                        groundPosition.y + 10.0*(position.y - 0.5),
                         1.0);
    pos.y = scaleMatrix[1][1]*height(vec2((pos.x + 512.5) / 1025.0, (pos.z + 512.5) / 1025.0)) + 0.5;
    texcoo = position;
    gl_Position = vpMatrix * pos;
}
