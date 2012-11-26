in vec3 vertexPosition;
in vec3 vertexColor;

out vec3 Color;

void main()
{
    Color = vertexColor;
    gl_Position = vec4(vertexPosition, 1.0);
}
