#version 150

in Vertex
{
	vec4 Pos;
	vec4 Normal;
	vec4 Colour; 
	vec2 UV;
	vec3 HalfDir;
	vec3 LightDir;
}vertex;

//three out put parameters for the render targets
out vec4 RT_Diffuse;
out vec4 RT_Normal;
out vec4 RT_World;



uniform sampler2D DiffuseTexture;
 
 struct Light{
 vec3 ambient;
 vec3 diffuse;
 vec3 specular;
 };

 const Light sceneLight = Light(vec3( 0.25, 0.25,0.25), vec3(1.0,1.0,1.0), vec3(1.0,1.0,1.0));
 const float specularTerm = 32.0;

void main()
{

	RT_Diffuse = vec4( 1.0, 0.4, 0.4, 1.0 );
	RT_Normal = vec4((vertex.Normal.xyz + 1.0) * 0.5, 1.0);
	RT_World = vertex.Pos;
}
