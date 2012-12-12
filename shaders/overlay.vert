#version 400

uniform vec2 pixelSize;
uniform vec2 screenPosition;

layout (location = 0) in vec2 position;

void main()
{
    gl_Position = vec4(screenPosition + 2.0*pixelSize*position, 0.0, 1.0);
}
