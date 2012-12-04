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
    vec4 tColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 splatSample = texture2D(splatTexture, tex);
    tColor += splatSample.r * texture2D(texture1, vec2(mod(tex* 10.0, 1.0)));
    tColor += splatSample.g * texture2D(texture2, vec2(mod(tex * 10.0, 1.0)));
    tColor += splatSample.b * texture2D(texture3, vec2(mod(tex * 10.0, 1.0)));
    return tColor / (splatSample.r + splatSample.g + splatSample.b);
}

void main()
{
    float lightFraction = max(0.0,dot(normalize(normalOut), normalize(vec3(1.0, 1.0, 0.0))));
    //Ambient
    lightFraction = min(lightFraction + 0.20, 1.0);

    FragColor = terrainColor(texCoo);

    if(posOut.y  < -150.0)
    {
        float delta = max(posOut.y + 190.0, 0.0) / 40.0;
        float delta2 = max(posOut.y + 160.0, 0.0) / 10.0;

        //0 <= delta < 1
        //For 0 we want max blur. For 1 we want no blur

        vec4 blur = vec4(0.0, 0.0, 0.0, 0.0);
        for(float i = -2.0f; i <= 2.0f; ++i)
            for(float j = -2.0f; j <= 2.0f; ++j)
                blur += terrainColor(texCoo + vec2(i/1024.0, j/1024.0));
        blur /= 25.0;

        FragColor = delta * FragColor + (1.0 - delta)*blur;

        FragColor *= delta2 + (1.0 - delta2)*vec4(0.2, 0.5, 1.0, 1.0);
    }

    FragColor *= lightFraction;
}
