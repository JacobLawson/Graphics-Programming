#version 330

in vec4 vPosition;
in vec4 vColour;
in vec4 vNormal;
in vec2 vTexCoord;

out vec4 fragColour;

uniform vec4 u_cameraPosition;
uniform vec4 u_lightDirection;
uniform vec4 u_lightColour;

uniform vec4 u_matAmbient;
uniform vec4 u_matDiffuse;
uniform vec4 u_matSpecular;

uniform sampler2D u_diffuseTexture;

const vec4 ambientLight = vec4(0.4f, 0.4f, 0.4f, 1.f);
const vec4 lightSpecular = vec4(1.f, 1.f, 1.f, 1.f);

//use a uniform to get cpu side data into the shader

void main()
{
	vec4 albedo = texture(u_diffuseTexture, vTexCoord);
	vec4 ambient = ambientLight * u_matAmbient;
	//difuse Light
	//diffuse * diffuse light colour * n.l
	vec4 diffuse = u_matDiffuse * u_lightColour * max(0.f, dot(vNormal, -u_lightDirection));
	vec4 reflectionVec = reflect(u_lightDirection, vNormal);
	vec4 eyeDir = normalize(u_cameraPosition - vPosition);
	vec4 specular = u_matSpecular * lightSpecular * max(0.f, pow(dot(eyeDir, reflectionVec), 125.f));
	fragColour = albedo * (ambientLight + diffuse + specular);
}