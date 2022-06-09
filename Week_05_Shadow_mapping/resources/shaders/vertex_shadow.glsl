#version 150

in vec4 Position;
in vec4 Colour;
in vec4 Normal;
in vec2 Tex1;

uniform mat4 shadowProjectionView;
uniform mat4 Model;

void main()
{
	gl_Position = shadowProjectionView * Model * Position;
}