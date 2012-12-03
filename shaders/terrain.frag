#version 400

uniform sampler2D splatTexture;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

in vec2 texCoo;
out vec4 FragColor;

vec4 terrainColor()
{
    vec4 tColor;
    vec4 splatSample = texture2D(splatTexture, texCoo);
    tColor += splatSample.r * texture2D(texture1, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.g * texture2D(texture2, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.b * texture2D(texture3, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.a * texture2D(texture4, vec2(mod(texCoo * 10.0, 1.0)));
    return 0.25*tColor;
}

void main()
{
    FragColor = terrainColor();
}
