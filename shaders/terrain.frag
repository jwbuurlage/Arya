#version 400

uniform sampler2D splatTexture;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

in vec2 texCoo;
in vec4 posOut;
out vec4 FragColor;

vec4 terrainColor()
{
    vec4 tColor;
    vec4 splatSample = texture2D(splatTexture, texCoo);
    tColor += splatSample.r * texture2D(texture1, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.g * texture2D(texture2, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.b * texture2D(texture3, vec2(mod(texCoo * 10.0, 1.0)));
    return tColor / (splatSample.r + splatSample.g + splatSample.b);
}

void main()
{
    if(posOut.y  < -135.0)
        FragColor = normalize(terrainColor() * vec4(0.2, 0.5, 1.0, 1.0));
    else
        FragColor = terrainColor();
}
