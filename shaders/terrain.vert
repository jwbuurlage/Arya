#version 140 

uniform sampler2D heightMap;
uniform mat4 vpMatrix;
uniform mat4 scaleMatrix;

uniform vec2 patchPosition;
uniform vec2 patchOffset;

layout (location = 0) in vec2 texCooPatch;
out vec2 texCoo;
out vec4 posOut;
out vec3 normalOut;

float height(vec2 tco)
{
    vec4 h = vec4(0.0);
    h = texture(heightMap, tco);
    return h.r;
}

void main()
{
    texCoo = patchOffset + texCooPatch;
    vec4 pos = scaleMatrix * vec4(texCoo.x-0.5, height(texCoo), texCoo.y-0.5, 1.0);

    float textureDelta = 1.0/512.0;
    float A = height(texCoo + vec2(0.0,textureDelta));
    float B = height(texCoo + vec2(textureDelta,0.0));
    float C = height(texCoo + vec2(0.0,-textureDelta));
    float D = height(texCoo + vec2(-textureDelta,0.0));

    vec4 normal = vec4( (D-B), 2.0/scaleMatrix[1][1], (C-A), 0.0 );
    normalOut = normalize( normal.xyz );

    gl_Position = vpMatrix * pos;
    posOut = pos;
}

