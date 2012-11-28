#version 400

uniform sampler2D heightMap;
uniform mat4 translationMat;

uniform vec2 patchPosition;
uniform vec2 patchOffset;

in vec2 position;

void main()
{
    vec4 pos = vec4(position, texture2D(heightMap, patchOffset+(0.1*position)).r, 1.0);

    gl_Position = pos;
}
