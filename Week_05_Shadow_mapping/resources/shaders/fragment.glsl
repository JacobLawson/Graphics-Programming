#version 150

in Vertex
{
    vec4 Pos;
    vec4 Normal;
    vec4 Colour; 
    vec2 UV;
    vec3 HalfDir;
    vec3 LightDir;
    vec4 ShadowCoord;
}vertex;

out vec4 FragColor;

uniform sampler2D DiffuseTexture;
uniform sampler2DShadow ShadowTexture;
 
struct Light{
vec3 ambient;
vec3 diffuse;
vec3 specular;
};

const Light sceneLight = Light(vec3( 0.125, 0.125,0.125), vec3(1.0,1.0,1.0), vec3(1.0,1.0,1.0));
const float specularTerm = 32.0;
const float SHADOW_BIAS = 0.005f;

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float Random(in vec4 seed4)
{   
    float dotprod = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dotprod) * 43758.5453);
}

void main()
{
    float shadowFactor = 0.5;
    float cosTheta = max(0,dot(vertex.Normal.xyz, vertex.LightDir));
    float bias = SHADOW_BIAS*tan(acos(cosTheta));
    bias = clamp(bias, 0, 0.01);
    
    //poissonDisk sampling
    for (int i=0;i<4;i++)
    {
        if (texture(sampler2D(ShadowTexture), vertex.ShadowCoord.xy + poissonDisk[i]/700.0 ).z  <  vertex.ShadowCoord.z-bias )
        {
            int index = int(16.0*Random(vec4(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.y, i)))%16;
            shadowFactor -= 0.1 * (1.0 - texture(sampler2D(ShadowTexture), vec3(vertex.ShadowCoord.xy + poissonDisk[index]/700.0, (vertex.ShadowCoord.z-bias)/vertex.ShadowCoord.w).xy)).z-bias;         
        }
    }

    vec3 albedo = texture(DiffuseTexture, vertex.UV).xyz;

    //Phong ambient color
    vec3 ambient = sceneLight.ambient;
    //Phong Diffuse
    vec3 diffuse = max(0,dot(vertex.Normal.xyz, vertex.LightDir))* sceneLight.diffuse;

    //Calculate Specular Component
    vec3 specular = pow(max(0,dot(vertex.HalfDir, vertex.Normal.xyz)),specularTerm)*sceneLight.specular;

    vec3 linearColour = albedo * ambient + albedo * (diffuse + specular) * shadowFactor; 
    //gamma correction
    vec3 gammaCorrected = pow(linearColour, vec3(1.0/2.2));
    FragColor = vec4(gammaCorrected, 1.0f);
}