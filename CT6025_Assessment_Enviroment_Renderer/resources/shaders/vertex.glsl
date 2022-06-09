#version 150

in vec4 Position;
in vec4 Colour;
in vec4 Normal;
in vec2 Tex1;

out Vertex
{
    vec4 Pos;
    vec4 Normal;
    vec4 Colour; 
    vec2 UV;
    vec3 HalfDir;
    vec3 LightDir;
    vec4 ShadowCoord;
}vertex;

out DirectionalLight
{
   vec4 lightDiffuse;
   vec4 lightAmbient;
   vec4 lightSpecular;
   vec4 lightDirection;
}directionalLight;

out PointLight
{
   vec4 lightPosition;

   vec4 lightDiffuse;
   vec4 lightAmbient;
   vec4 lightSpecular;

   float lightConstant;
   float lightLinear;
   float lightQuadratic;

}pointLight;

out SpotLight
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


out Matrix{
    mat4 Model;
    mat4 Normal;
    mat4 View;
    mat4 projection;
}matrix;

uniform mat4 ProjectionView; 
uniform mat4 Model;
uniform mat4 NormalMatrix;
uniform mat4 ViewMatrix;
uniform mat4 shadowProjectionView;

uniform vec4 cameraPosition;

//Directioal Light
uniform vec4 uLightDirection;
uniform vec4 uLightDiffuse;
uniform vec4 uLightAmbient;
uniform vec4 uLightSpecular;

//point light
uniform vec4 uPointLightPosition;
uniform vec4 uPointLightDiffuse;
uniform vec4 uPointLightAmbient;
uniform vec4 uPointLightSpecular;
uniform float uLightConstant;
uniform float uLightLinear;
uniform float uLightQuadratic;

//spot light
uniform vec4 uSpotLightPosition;
uniform vec4 uSpotLightDirection;
uniform vec4 uSpotLightDiffuse;
uniform vec4 uSpotLightAmbient;
uniform vec4 uSpotLightSpecular;
uniform float uSpotLightConstant;
uniform float uSpotLightLinear;
uniform float uSpotLightQuadratic;
uniform float uSpotLightAngle;

const mat4 SHADOW_BIAS_MATRIX = mat4(
                            0.5, 0.0, 0.0, 0.0,
                            0.0, 0.5, 0.0, 0.0,
                            0.0, 0.0, 0.5, 0.0,
                            0.5, 0.5, 0.5, 1.0
                            );

void main() 
{ 
    //directional light
    directionalLight.lightDirection = uLightDirection;
    directionalLight.lightDiffuse = uLightDiffuse;
    directionalLight.lightAmbient = uLightAmbient;
    directionalLight.lightSpecular = uLightSpecular;

    //Point light
    pointLight.lightPosition = uPointLightPosition;
    pointLight.lightDiffuse = uPointLightDiffuse;
    pointLight.lightAmbient = uPointLightAmbient;
    pointLight.lightSpecular = uPointLightSpecular;
    pointLight.lightConstant = uLightConstant;
    pointLight.lightLinear = uLightLinear;
    pointLight.lightQuadratic = uLightQuadratic;

    //Spotlight
    spotLight.lightPosition = uSpotLightPosition;
    spotLight.lightDirection = uSpotLightDirection;
    spotLight.lightDiffuse = uSpotLightDiffuse;
    spotLight.lightAmbient = uSpotLightAmbient;
    spotLight.lightSpecular = uSpotLightSpecular;
    spotLight.lightConstant = uSpotLightConstant;
    spotLight.lightLinear = uSpotLightLinear;
    spotLight.lightQuadratic = uSpotLightQuadratic;
    spotLight.angle = uSpotLightAngle;

    //light vertex
    vertex.Pos = Model * Position;
    vertex.Normal = ViewMatrix * NormalMatrix * Normal;
    vertex.Colour = Colour;
    vertex.UV = Tex1;

    vertex.LightDir = (ViewMatrix * -directionalLight.lightDirection).xyz;

    vec3 eyeDir = (ViewMatrix * normalize( cameraPosition - vertex.Pos)).xyz;
    vertex.HalfDir = normalize(eyeDir + vertex.LightDir);

    vertex.ShadowCoord = SHADOW_BIAS_MATRIX * shadowProjectionView * Position;

    gl_Position = ProjectionView * Model * Position;
}