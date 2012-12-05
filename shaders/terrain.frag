#version 400

uniform sampler2D splatTexture;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

in vec2 texCoo;
in vec4 posOut;
in vec3 normalOut;
out vec4 FragColor;

vec4 terrainColor(vec2 tex)
{
    vec4 tColor = vec4(0.0);
    vec4 splatSample = vec4(0.0);
    splatSample = texture(splatTexture, texCoo);
    tColor += splatSample.r * texture(texture1, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.g * texture(texture2, vec2(mod(texCoo * 10.0, 1.0)));
    tColor += splatSample.b * texture(texture3, vec2(mod(texCoo * 10.0, 1.0)));
    return tColor / (splatSample.r + splatSample.g + splatSample.b);
}

void main()
{
    float lightFraction = max(0.0,dot(normalize(normalOut), normalize(vec3(1.0, 1.0, 0.0))));
    //Ambient
    lightFraction = min(lightFraction + 0.20, 1.0);

    FragColor = terrainColor(texCoo);

    if(posOut.y  < -150.0)
        FragColor *= vec4(0.2, 0.5, 1.0, 1.0);
    else
        FragColor *= lightFraction;
}
