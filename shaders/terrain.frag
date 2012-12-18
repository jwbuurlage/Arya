#version 400

uniform sampler2D splatTexture;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D shadowMap;

uniform vec4 parameters1; //specAmp, specPow, ambient, diffuse
uniform vec4 parameters2;
uniform vec4 parameters3;
uniform vec4 parameters4;

uniform mat4 lightOrthoMatrix;

in float spec;
in vec2 texCoo;
in vec4 posOut;
in vec3 normalOut;
out vec4 FragColor;

void main()
{
	vec3 lightDirection=vec3(0.7,0.7,0.0);//MUST BE REPLACED
	
    float lightFraction = max(0.0,dot(normalize(normalOut), lightDirection));

	vec4 tColor = vec4(0.0);
    vec4 splatSample = vec4(0.0);
    splatSample = texture(splatTexture, texCoo);
	
	vec4 color1 = texture(texture1, 10.0*texCoo);
	vec4 color2 = texture(texture2, 10.0*texCoo);
	vec4 color3 = texture(texture3, 10.0*texCoo);

	color1 *= max(lightFraction*parameters1.w, parameters1.z);
	color2 *= max(lightFraction*parameters2.w, parameters2.z);
	color3 *= max(lightFraction*parameters3.w, parameters3.z);

	//FragColor=vec4(parameters1.x,parameters1.y,parameters1.z,1.0);
	//FragColor.xyz=color1.xyz;

	color1.xyz += parameters1.x*vec3(pow(spec,parameters1.y));
	color2.xyz += parameters2.x*vec3(pow(spec,parameters2.y));
	color3.xyz += parameters3.x*vec3(pow(spec,parameters3.y));

	FragColor=vec4(0.0, 0.0, 0.0, 1.0);
	FragColor.xyz+=splatSample.r*color1.xyz+splatSample.g*color2.xyz+splatSample.b*color3.xyz;
	FragColor.xyz/=(splatSample.r + splatSample.g + splatSample.b);
	FragColor.a=1.0;

    vec4 posOnShadowTex = lightOrthoMatrix * posOut;

    if(!(posOnShadowTex.x < 0.0 || posOnShadowTex.x > 1.0))
        if(!(posOnShadowTex.y < 0.0 || posOnShadowTex.y > 1.0)) 
            if(!(posOnShadowTex.z < 0.0 || posOnShadowTex.z > 1.0)) 
                if(texture(shadowMap, posOnShadowTex.xy).r < posOnShadowTex.z)
                    FragColor *= 0.5;

}
