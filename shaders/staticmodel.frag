#version 140
#extension GL_ARB_explicit_attrib_location : require

uniform sampler2D tex;
uniform vec4 parameters;//specAmp, specPow, ambient, diffuse
uniform vec3 tintColor;

in vec2 texCoo;
in vec3 normal;
in float spec;

layout (location = 0) out vec4 fragColor;

void main()
{
	vec3 lightDirection=vec3(0.7,0.7,0.0);//MUST BE REPLACED

	float lightFraction = max(0.0,dot(normalize(normal), lightDirection));
	fragColor = texture(tex, texCoo);
	if(fragColor.xyz == vec3(1.0, 0.0, 1.0))
        fragColor.xyz = tintColor;
	//fragColor.xyz=vec3(1.0,0.5,1.0);
	fragColor.xyz *= max(lightFraction*parameters.w, parameters.z);
	fragColor.xyz += parameters.x*vec3(pow(spec,parameters.y));
	fragColor.a=1.0;
}
