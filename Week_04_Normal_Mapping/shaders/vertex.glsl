#version 330

in vec4 Position;
in vec4 Colour;
in vec4 Normal;
in vec4 Tangent;
in vec4 BiTangent;
in vec2 Tex1;

out vec4 vPosition;
out vec4 vColour;
out vec4 vNormal;
out vec2 vUV;
out vec4 vLightDir;
out vec4 vViewDir;

uniform int u_useNormalMap = 1;

uniform mat4 u_projectionView;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;
uniform mat4 u_normalMatrix;

uniform vec4 u_cameraPosition;
uniform vec4 u_lightDirection;

void main()
{
	vPosition = u_modelMatrix * Position;
	vNormal = u_normalMatrix * Normal;

	vec4 N = u_normalMatrix * Normal;
	vec4 T = u_normalMatrix * Tangent;
	vec4 B = u_normalMatrix * BiTangent;

	mat3 TBN = transpose(mat3(T.xyz, B.xyz, N.xyz));
	vec4 E = normalize(( u_modelMatrix * u_cameraPosition) - vPosition);
	if (u_useNormalMap == 1)
	{
		vViewDir = normalize(vec4(TBN * E.xyz, 0.f));
		vLightDir = normalize(vec4(TBN * (u_modelMatrix * u_lightDirection).xyz, 0.f));
	}
	else
	{
		vViewDir = normalize(vec4(E.xyz, 0.f));
		vLightDir = normalize(vec4(( u_modelMatrix * u_lightDirection).xyz, 0.f));
	}
	vColour = Colour;
	vUV = Tex1;
	gl_Position = u_projectionView * u_modelMatrix * Position;
}