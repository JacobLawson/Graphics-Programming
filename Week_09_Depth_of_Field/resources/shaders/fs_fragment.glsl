#version 150

in vec2 UV;
	
out vec4 FragColor;
uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;
uniform sampler2D WorldTexture;

uniform vec3 camera_pos;
uniform vec2 screenSize;

uniform vec3 lightDir;


struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

const Light sceneLight = Light(vec3(0.25, 0.25, 0.25), vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0));
const float specularTerm = 32.0;

void main()
{
	//gl_FragCoord is a build in frag shader variable that contains
	//the falue of the pixel currently being drawn. 
	//Can convert to 0-1 uv coords by dividing with screen dimension
	vec2 fUV = gl_FragCoord.xy / screenSize;

	vec4 DiffuseColour = texture(DiffuseTexture, fUV);
	vec3 Normal = normalize((texture(NormalTexture, fUV).xyz * 2.0) - 1.0);
	vec3 WorldPos = texture(WorldTexture, fUV).xyz;
	
	vec3 Eye = normalize(camera_pos - WorldPos);
	vec3 Light = normalize(lightDir);

	vec3 albedo = DiffuseColour.xyz;

	//Phong ambient color
	vec3 ambient = sceneLight.ambient;
	//Phong Diffuse
	vec3 diffuse = max(0, dot(Normal.xyz, -lightDir)) * sceneLight.diffuse;

	//Calculate Specular Component
	vec3 specular = pow(max(0, dot(Eye, Normal.xyz)), specularTerm)*sceneLight.specular;

	vec3 linearColour = albedo * (ambient + diffuse + specular);
	//gamma correction
	vec3 gammaCorrected = pow(linearColour, vec3(1.0 / 2.2));
	FragColor = vec4(gammaCorrected, 1.0f);

}
