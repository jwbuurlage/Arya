#version 400

uniform sampler2D clouds;
in vec3 normalVector;
in vec2 texCoord;

in float spec;
in float depth;

out vec4 FragColor;

vec4 terrainColor(vec2 tex)
{
    return texture(clouds, 3.*(tex-.001*normalVector.xz));
}

void main()
{
	float refFact=0.1*pow(spec,0.2)+0.2*pow(spec,2.0)+10.0*pow(spec,100.0);
	vec3 waterColor=.5*terrainColor(texCoord).xyz+.5*vec3(0.0862745, 0.266667, 0.678431);
	//if(0.2+min(10.0*depth,1.0) < .5) discard;
	FragColor=vec4(waterColor, 0.2+min(10.0*depth,1.0))+refFact*vec4(waterColor, 0);
}