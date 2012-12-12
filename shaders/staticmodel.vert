#verison 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCooIn;
layout (location = 2) in vec3 normalIn;

out vec2 texCoo;
out vec2 normal;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;

void main()
{
    texCoo = texCooIn;
    normal = normalIn;
    gl_Position = vpMatrix * mMatrix * position;
}
