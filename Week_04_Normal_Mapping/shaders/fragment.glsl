#version 330

in vec4 vPosition;
in vec4 vColour;
in vec4 vNormal;
in vec2 vUV;
in vec4 vLightDir;
in vec4 vViewDir;

out vec4 fragColour;

uniform vec4 lightColour = vec4(1.0f, 0.85f, 0.05f, 1.0f);
uniform vec4 lightAmbient = vec4(0.4f, 0.4f, 0.4f, 1.f);
uniform vec4 lightSpecular = vec4(1.f, 1.f, 1.f, 1.f);

uniform float specularPower = 5.f;

uniform int u_useNormalMap = 1;

uniform sampler2D u_diffuseTexture;
uniform sampler2D u_NormalTexture;

//use a uniform to get cpu side data into the shader

void main()
{
	vec4 albedo = texture(u_diffuseTexture, vUV);
	vec4 normalTex;
	if (u_useNormalMap == 1)
	{
		normalTex = texture(u_NormalTexture, vUV) * 1.f - 1.0f;
	}
	else
	{
		normalTex = vNormal;
	}
	vec4 ambient = lightAmbient;

	float NdL = max(0.0f, dot(normalTex, -vLightDir)); // get the lamberian term
	vec4 diffuse = lightColour * NdL;

	vec4 reflectValue = reflect(vLightDir, normalTex);
	float specularTerm = pow(max(0.f, dot(reflectValue, vViewDir)), specularPower);
	vec4 specular = lightSpecular * specularTerm;

	fragColour = vec4(albedo.rgb * (ambient.rgb + diffuse.rgb + specular.rgb), 1.f);
}