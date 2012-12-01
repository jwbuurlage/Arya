#version 400

uniform sampler2d splatTexture;
uniform sampler2d texture1;
uniform sampler2d texture2;
uniform sampler2d texture3;
uniform sampler2d texture4;

out vec4 FragColor;

vec4 terrainColor()
{
    return texture2d(texture1, vec2(0.0, 0.0));
}

void main()
{
    FragColor = terrainColor;
}
