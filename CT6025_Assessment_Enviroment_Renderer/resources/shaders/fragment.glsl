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

in Matrix{
    mat4 Model;
    mat4 Normal;
    mat4 View;
    mat4 projection;
}matrix;

in DirectionalLight
{
   vec4 lightDiffuse;
   vec4 lightAmbient;
   vec4 lightSpecular;
   vec4 lightDirection;
}directionalLight;

in PointLight
{
   vec4 lightPosition;

   vec4 lightDiffuse;
   vec4 lightAmbient;
   vec4 lightSpecular;

   float lightConstant;
   float lightLinear;
   float lightQuadratic;

}pointLight;

in SpotLight
{
   vec4 lightPosition;
   vec4 lightDirection;

   vec4 lightDiffuse;
   vec4 lightAmbient;
   vec4 lightSpecular;

   float angle;
   float lightConstant;
   float lightLinear;
   float lightQuadratic;
}spotLight;

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

//directional light
vec3 calculateDirectionalLight()
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
    vec4 ambient = directionalLight.lightAmbient;
    //Phong Diffuse
    vec3 diffuse = max(0,dot(vertex.Normal.xyz, -directionalLight.lightDirection.xyz))* directionalLight.lightDiffuse.xyz;

    //Calculate Specular Component
    vec3 specular = pow(max(0,dot(vertex.HalfDir, vertex.Normal.xyz)), directionalLight.lightSpecular.w)*directionalLight.lightSpecular.xyz;

    vec3 linearColour = albedo.xyz * ambient.xyz + albedo * (diffuse.xyz + specular) * shadowFactor; 
    //gamma correction
    vec3 gammaCorrected = pow(linearColour, vec3(1.0/2.2));

    return gammaCorrected;
}

//Point Light
vec3 calculatePointLight()
{
    vec3 lightDir = normalize(vertex.Pos.xyz - (matrix.Model * pointLight.lightPosition).xyz);
 
    vec3 albedo = texture(DiffuseTexture, vertex.UV).xyz;
    vec3 diffuse = (max(0,dot(vertex.Normal.xyz, lightDir)) * pointLight.lightDiffuse).xyz;
    vec3 ambient = pointLight.lightAmbient.xyz;
    // specular shading
    vec3 reflectDir = reflect(-lightDir, vertex.Normal.xyz);
    vec3 specular = pow(max(dot(vertex.HalfDir, reflectDir), 0.0), pointLight.lightSpecular.w) * pointLight.lightSpecular.xyz * pointLight.lightDiffuse.xyz;
    // attenuation
    float distance = length((matrix.Model * pointLight.lightPosition).xyz - vertex.Pos.xyz);
    float attenuation = 1.0 / (pointLight.lightConstant + pointLight.lightLinear * distance + pointLight.lightQuadratic * (distance * distance));    
    // combine results

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    vec3 lightResult = albedo * ambient + albedo * (diffuse + specular);
    
    return lightResult;
}

//SpotLight
vec3 CalculateSpotLight()
{
    vec3 lightDir = normalize(vertex.Pos.xyz - (matrix.Model * spotLight.lightPosition).xyz);

    float factor = dot(lightDir, normalize(spotLight.lightDirection).xyz);
    if (factor > spotLight.angle)
    {
        vec3 albedo = texture(DiffuseTexture, vertex.UV).xyz;
        vec3 diffuse = (max(0,dot(vertex.Normal.xyz, lightDir)) * spotLight.lightDiffuse).xyz;
        vec3 ambient = spotLight.lightAmbient.xyz;
        // specular shading
        vec3 reflectDir = reflect(-lightDir, vertex.Normal.xyz);
        vec3 specular = pow(max(dot(vertex.HalfDir, reflectDir), 0.0), spotLight.lightSpecular.w) * spotLight.lightSpecular.xyz * spotLight.lightDiffuse.xyz;
        // attenuation
        float distance = length((matrix.Model * spotLight.lightPosition).xyz - vertex.Pos.xyz);
        float attenuation = 1.0 / (spotLight.lightConstant + spotLight.lightLinear * distance + spotLight.lightQuadratic * (distance * distance));    
        
        // combine results
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
        vec3 lightResult = albedo * ambient + albedo * (diffuse + specular);
        
        float lightIntensity = (1.0f - (1.0f - factor)/(1.0f - spotLight.angle));
        return lightResult * lightIntensity;
    }
        return vec3(0.f);
}

void main()
{
    vec3 result = vec3(0.0f);
    result = calculateDirectionalLight();
    result += calculatePointLight();
    result += CalculateSpotLight();
   
    FragColor = vec4(result, 1.0f);
}