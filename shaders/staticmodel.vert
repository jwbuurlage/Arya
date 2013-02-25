#version 140
#extension GL_ARB_explicit_attrib_location : require

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCooIn;
layout (location = 2) in vec3 normalIn;
layout (location = 3) in vec3 posNext;
layout (location = 4) in vec3 normalNext;

out vec2 texCoo;
out vec3 normal;
out float spec;

uniform mat4 mMatrix;
uniform mat4 viewMatrix;
uniform mat4 vpMatrix;
uniform float interpolation;
uniform vec4 parameters;//specAmp, specPow, ambient, diffuse

void main()
{
	vec3 lightDirection=vec3(0.7,0.7,0.0);//MUST BE REPLACED

    texCoo = texCooIn;
	vec3 norm=normalize((mMatrix*vec4( (1.0 - interpolation)*normalIn + interpolation*normalNext , 0.0)).xyz);
    

    vec3 pos = (1.0 - interpolation) * position + interpolation*posNext;

	if(parameters[0] > 0.001) {
		vec4 camNormal=normalize(viewMatrix*vec4(norm,0.0));
		vec4 camLight=normalize(viewMatrix*vec4(lightDirection,0.0));
		vec4 camReflection=2.0*camNormal*dot(camLight,camNormal)-camLight;
		spec=max(dot(camReflection,-1.0*normalize(viewMatrix*vec4(pos,0.0))),0);
	} else spec=0.0;
	normal = norm;
    gl_Position = vpMatrix * mMatrix * vec4(pos,1.0);
}
