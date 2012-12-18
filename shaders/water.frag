#version 400

uniform sampler2D clouds;
uniform float time;

in vec3 normalVector;
in vec2 texCoord;

in float spec;
in float depth;

out vec4 FragColor;

vec4 terrainColor(vec2 tex)
{
	tex=tex-time*0.01*vec2(.3, .4);
    return texture(clouds, 3.*(tex-.001*normalVector.xz));
}

void main()
{
	float refFact=0.1*pow(spec,0.2)+0.2*pow(spec,2.0)+10.0*pow(spec,100.0);
	vec3 waterColor=.3*terrainColor(texCoord).xyz+.7*vec3(0.125882, 0.12549, 0.537255);//0.0862745, 0.266667, 0.678431);
	if(0.2+min(10.0*depth,1.0) < .1) discard;
	FragColor=vec4(waterColor, 0.2+min(10.0*depth,1.0))+refFact*vec4(waterColor, 0);
}
