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
    vec4 pos = vec4(patchPosition + scaleMatrix * texCoo, texture2D(heightMap, texCoo).r, 1.0);

    gl_Position = vpMatrix * pos;
}
