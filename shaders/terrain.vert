#version 140
#extension GL_ARB_explicit_attrib_location : require

uniform sampler2D splatTexture;
uniform sampler2D heightMap;
uniform sampler2D fogMap;
uniform mat4 vpMatrix;
uniform mat4 scaleMatrix;
uniform mat4 viewMatrix;

uniform vec3 lightDirection;
uniform vec2 patchOffset;

layout (location = 0) in vec2 texCooPatch;
out vec2 texCoo;
out vec4 posOut;
out vec3 normalOut;
out float spec;

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

    float textureDelta = 0.001953125; // 1/512
    float A = height(texCoo + vec2(0.0,textureDelta));
    float B = height(texCoo + vec2(textureDelta,0.0));
    float C = height(texCoo + vec2(0.0,-textureDelta));
    float D = height(texCoo + vec2(-textureDelta,0.0));

    vec4 normal = vec4( (D-B), 2.0/scaleMatrix[1][1], (C-A), 0.0 );
	normalOut = normalize( normal.xyz );

	vec4 camNormal=normalize(viewMatrix*vec4(normal.xyz,0.0));
	vec4 camLight=normalize(viewMatrix*vec4(lightDirection,0.0));
	vec4 camReflection=2.0*camNormal*dot(camLight,camNormal)-camLight;
	spec=max(dot(camReflection,-1.0*normalize(viewMatrix*pos)),0);

    gl_Position = vpMatrix * pos;
    posOut = pos;
}

