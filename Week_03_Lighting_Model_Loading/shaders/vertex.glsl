#version 330

in vec4 Position;
in vec4 Colour;
in vec4 Normal;
in vec2 TexCoord;

out vec4 vPosition;
out vec4 vColour;
out vec4 vNormal;
out vec2 vTexCoord;

uniform mat4 u_projectionView;
uniform mat4 u_modelMatrix;
uniform mat4 u_normalMatrix;

void main()
{
	vColour = Colour;
	vNormal = normalize(u_normalMatrix * Normal);
	vPosition = u_modelMatrix * Position;
	vTexCoord = TexCoord;
	gl_Position = u_projectionView * u_modelMatrix * Position;
}