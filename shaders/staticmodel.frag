#version 140

uniform sampler2D tex;

in vec2 texCoo;
in vec3 normal;

layout (location = 0) out vec4 fragColor;

void main()
{
    float lightFraction = max(0.0,dot(normalize(normal), vec3(0.7, 0.7, 0.0)));
    fragColor = lightFraction * texture(tex, texCoo);
    fragColor.a = 1.0;
}
