#version 150 core

in vec2 position;
in vec2 iFragCoord;

out vec2 fragCoord;
void main()
{
	fragCoord = iFragCoord;
    gl_Position = vec4(position, 0.0, 1.0);
}