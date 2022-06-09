#version 150

in vec4 Position;
in vec2 Tex1;

out vec2 UV;


void main() 
{ 
	UV = Tex1;
	gl_Position = Position;
}