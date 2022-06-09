#version 150

in vec4 Position;
in vec4 Colour;
in vec4 Normal;
in vec4 Tangent;
in vec4 BiTangent;
in vec2 Tex1;

out Vertex
{
	mat3 TBN;
	vec4 Pos;
	vec4 Normal;
	vec4 Colour; 
	vec2 UV;
}vertex;

uniform mat4 ProjectionView; 
uniform mat4 Model;
uniform mat4 NormalMatrix;
uniform mat4 ViewMatrix;

uniform vec4 cameraPosition;
uniform vec4 lightDirection;

void main() 
{ 
	//calculate our TBN Matrix, do this in view space
	vec4 N = NormalMatrix * Normal;
	vec4 T = NormalMatrix * Tangent;
	vec4 B = NormalMatrix * BiTangent;

	vertex.TBN = mat3(T.xyz, B.xyz, N.xyz);

	vertex.Pos = ViewMatrix * Model * Position;
	vertex.Normal = N;
	vertex.Colour = Colour;
	vertex.UV = Tex1;

		
	gl_Position = ProjectionView * Model * Position;
}