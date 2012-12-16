#version 140

uniform sampler2D heightMap;
uniform sampler2D waterMap;
uniform mat4 vpMatrix;
uniform mat4 viewMatrix;
uniform mat4 scaleMatrix;

uniform vec3 lightSource;
uniform vec2 patchPosition;
uniform vec2 patchOffset;
uniform float time;

layout (location = 0) in vec2 texCooPatch;

out vec3 normalVector;
out vec2 texCoord;

out float spec;
out float depth;

float heightGround(vec2 tco)
{
    vec4 h = vec4(0.0);
    h = texture(heightMap, tco);
    return h.r;
}

float heightWater(vec2 tco)
{
    vec4 h = vec4(0.0);
    h = texture(waterMap, tco);
    return h.r;
}

float getWave(vec2 tco)
{
	return	.1*sin(tco.x*0.21 - time*1.02921)	+ .4*sin(tco.x*0.04156 - time*2.9553) + .5*sin(tco.x*0.021456 - time*1.9489) +
			.1*sin(tco.y*0.025442 - time*4.2352)	+ .4*sin(0.04139*tco.y - time*3.6950) + .6*sin(tco.y*0.1546 - time*1.943356);
}

void main()
{
	vec2 texCoo;
	texCoo = patchOffset + texCooPatch;
	texCoord=texCoo;
	//vec2 scaledPos = (scaleMatrix*vec4(vec2(-0.5)+texCoo, 0.0, 1.0)).xy;


	float h=0.0;
	float hw=heightWater(texCoo);
	float hg=heightGround(texCoo);
	float factor=0.0;
	
	depth=(hw-hg);
	float diff=(hw-hg)*10.0;
	
	if(hw > hg) {
		if(diff < 0.5) {
			factor=2*diff*diff;
		} else if(diff < 1.0) {
			factor=-1.0+4.0*diff-2.0*diff*diff;
		} else {
			factor=1.0;
		}
		h=getWave(1000.0*texCoo);
	}

	//vec4 pos = vec4(scaledPos.x, -100.0 + 200.0*(hw+factor*0.01*h), scaledPos.y, 1.0);

	vec4 pos = scaleMatrix * vec4(texCoo.x-0.5, hw+factor*0.01*h, texCoo.y-0.5, 1.0);
	//vec4 pos = scaleMatrix * vec4(texCoo.x-0.5, heightWater(texCoo), texCoo.y-0.5, 1.0);

	float infi=0.5;
	float dfx=getWave(texCoo+vec2(infi,0))-h;
	float dfy=getWave(texCoo+vec2(0,infi))-h;
	vec4 normal=vec4(-dfx*infi,infi*infi,-dfy*infi,0.0);

	vec4 norm=normalize(viewMatrix*vec4(normal.xyz,0.0));
	vec4 light=viewMatrix*vec4(lightSource.xyz,0.0);
	vec4 reflection=2.0*norm*dot(light,norm)-light;
	normalVector=reflection.xyz;
	spec=max(dot(reflection,-1.0*normalize(viewMatrix*pos)),0);

	gl_Position = vpMatrix * pos;
}
