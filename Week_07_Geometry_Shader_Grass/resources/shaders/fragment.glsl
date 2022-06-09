#version 150

in vec4 vColour;
in vec2 vUV;

out vec4 FragColor;

uniform sampler2D textureLoc;
uniform sampler2D textureAlpha;
 
//A simple shader that samples a UV coordinate from an attached texture 
// and multiplies that texture sample by a colour.
void main()
{
	FragColor =  texture2D(textureAlpha, vUV).rgba * texture2D( textureLoc, vUV).a; 
}
