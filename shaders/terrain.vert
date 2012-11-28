#version 400

uniform Sampler2D heightMap;
uniform mat4 translationMat;

uniform vec2 patchPosition;
uniform vec2 patchOffset;

in vec2 position;

void main()
{
    vec2 pos = translationMat * vec4(position, 0.0, 1.0);
    gl_Position(pos, 0.0, 1.0);
}
