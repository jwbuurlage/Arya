#version 140

uniform sampler2D tex;
uniform vec3 tintColor;

in vec2 texCoo;
in vec3 normal;

layout (location = 0) out vec4 fragColor;

void main()
{
    float lightFraction = max(0.0,dot(normalize(normal), vec3(0.7, 0.7, 0.0)));
    fragColor = texture(tex, texCoo);
    if(fragColor.xyz == vec3(1.0, 0.0, 1.0))
        fragColor.xyz = tintColor;
   fragColor.xyz *= lightFraction;

}
