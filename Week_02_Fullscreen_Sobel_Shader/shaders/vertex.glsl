#version 330

in vec4 position;
in vec4 colour;
in vec2 TexCoord;

out vec4 vColour;
out vec2 vUV;

void main()
{
	vUV = TexCoord;
	vColour = colour;
	gl_Position = position;
}