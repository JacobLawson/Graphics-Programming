#version 150

in Vertex
{
	mat3 TBN;
	vec4 Pos;
	vec4 Normal;
	vec4 Colour;
	vec2 UV;
}vertex;

//three out put parameters for the render targets
out vec4 RT_Diffuse;
out vec4 RT_Normal;
out vec4 RT_World;

uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;
 

void main()
{
	//All output is in view space
	RT_Diffuse = texture( DiffuseTexture, vertex.UV );
	vec3 normal = texture(NormalTexture, vertex.UV).xyz;
	if (length(normal) < 0.5)
	{
		normal = vertex.Normal.xyz;
	}
	else
	{
		normal = vertex.TBN * (normal * 2.0 - 1.0);
	}
	RT_Normal = vec4(normal , 1.0);
	RT_World = vertex.Pos;
}
