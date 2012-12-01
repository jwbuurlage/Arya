#version 400

uniform sampler2d splatTexture;
uniform sampler2d texture1;
uniform sampler2d texture2;
uniform sampler2d texture3;
uniform sampler2d texture4;

in vec2 texCoo;
out vec4 FragColor;

vec4 terrainColor()
{
    vec4 tColor;
    splatSample = texture2d(splatTexture, texCoo);
    tColor += splatSample.r * texture2d(texture1, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.g * texture2d(texture2, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.b * texture2d(texture3, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.a * texture2d(texture4, vec2(mod(texCoo * 10.0, 1.0)));
}

void main()
{
    FragColor = terrainColor();
}
