#version 400

uniform sampler2D splatTexture;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

in float spec;
in vec2 texCoo;
in vec4 posOut;
in vec3 normalOut;
out vec4 FragColor;

vec4 terrainColor(vec2 tex)
{
    vec4 tColor = vec4(0.0);
    vec4 splatSample = vec4(0.0);
    splatSample = texture(splatTexture, texCoo);
    tColor += splatSample.r * texture(texture1, 20.0*texCoo);
    tColor += splatSample.g * texture(texture2, 20.0*texCoo);
    tColor += splatSample.b * texture(texture3, 20.0*texCoo);
    return tColor / (splatSample.r + splatSample.g + splatSample.b);
}

void main()
{
    float lightFraction = max(0.0,dot(normalize(normalOut), vec3(0.7, 0.7, 0.0)));
    //Ambient
    lightFraction = min(lightFraction + 0.40, 1.0);

    FragColor = terrainColor(texCoo);
    FragColor *= lightFraction;
	FragColor += 1.0*vec4(pow(spec,6.0));
	FragColor.a=1.0f;
}
