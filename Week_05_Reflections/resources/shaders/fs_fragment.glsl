#version 150

in vec2 UV;
	
out vec4 FragColor;
uniform sampler2D DiffuseTexture;
uniform int windowWidth;
uniform int windowHeight;
void main()
{

	vec2 reflectedUV = gl_FragCoord.xy / vec2(windowWidth,windowHeight);
	FragColor = texture(DiffuseTexture, reflectedUV);
}
