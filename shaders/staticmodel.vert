#version 140

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCooIn;
layout (location = 2) in vec3 normalIn;
layout (location = 3) in vec3 posNext;
layout (location = 4) in vec3 normalNext;

out vec2 texCoo;
out vec3 normal;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform float interpolation;

void main()
{
    texCoo = texCooIn;
    normal = (1.0 - interpolation)*normalIn + interpolation*normalNext;
    vec3 pos = (1.0 - interpolation) * position + interpolation*posNext;
    gl_Position = vpMatrix * mMatrix * vec4(pos,1.0);
}
