#version 150

in vec2 uv;

out vec4 FragColour;

uniform sampler2D u_diffuseTexture;

void main()
{
	FragColour = texture(u_diffuseTexture, uv);
}