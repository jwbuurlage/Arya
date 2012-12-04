#version 400

uniform sampler2D heightMap;
uniform mat4 vpMatrix;
uniform mat4 scaleMatrix;

uniform vec2 patchPosition;
uniform vec2 patchOffset;

layout (location = 0) in vec2 texCooPatch;
out vec2 texCoo;
out vec4 posOut;
out vec3 normalOut;

void main()
{
    texCoo = patchOffset + texCooPatch;
    vec2 scaledPos = (scaleMatrix*vec4(vec2(-0.5)+texCoo, 0.0, 1.0)).xy;
    vec4 pos = vec4(scaledPos.x, -200.0 + 200.0*texture2D(heightMap, texCoo).r, scaledPos.y, 1.0);

    float textureDelta = 1/512.0;
    float A = texture2D(heightMap, texCoo + vec2(0.0,textureDelta)).r;
    float B = texture2D(heightMap, texCoo + vec2(textureDelta,0.0)).r;
    float C = texture2D(heightMap, texCoo + vec2(0.0,-textureDelta)).r;
    float D = texture2D(heightMap, texCoo + vec2(-textureDelta,0.0)).r;

    vec4 normal = vec4( (D-B), 2.0/200.0, (C-A), 0.0 );
    normalOut = normalize( normal.xyz );

    gl_Position = vpMatrix * pos;
    posOut = pos;
}

