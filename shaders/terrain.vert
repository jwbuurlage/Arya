#version 400

uniform sampler2D heightMap;
uniform mat4 vpMatrix;
uniform mat4 scaleMatrix;

uniform vec2 patchPosition;
uniform vec2 patchOffset;

layout (location = 0) in vec2 texCooPatch;
out vec2 texCoo;

void main()
{
    texCoo = patchOffset + texCooPatch;
    vec2 scaledPos = (scaleMatrix*vec4(vec2(-0.5)+texCoo, 0.0, 1.0)).xy;
    vec4 pos = vec4(scaledPos.x, -200.0 + 200.0*texture2D(heightMap, texCoo).r, scaledPos.y, 1.0);

    gl_Position = vpMatrix * pos;
}
