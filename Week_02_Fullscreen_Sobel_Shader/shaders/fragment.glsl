#version 330

in vec4 vColour;
in vec2 vUV;

out vec4 fragColour;

//use a uniform to get cpu side data into the shader
uniform sampler2D u_texture;
uniform float u_width;
uniform float u_height;

float Intensity(in vec3 a_rgb)
{
	return sqrt(dot(a_rgb, a_rgb));
}

void GetTexelKernel(out float a_pixel[8], in vec2 a_uv, in sampler2D a_texture)
{
	float w = 1.0 / u_width;
	float h = 1.0 / u_height;
	a_pixel[0] = Intensity(texture(a_texture, a_uv + vec2( -w,   h)).rgb);
	a_pixel[1] = Intensity(texture(a_texture, a_uv + vec2(0.0,   h)).rgb);
	a_pixel[2] = Intensity(texture(a_texture, a_uv + vec2(  w,   h)).rgb);
	a_pixel[3] = Intensity(texture(a_texture, a_uv + vec2( -w, 0.0)).rgb);
	a_pixel[4] = Intensity(texture(a_texture, a_uv + vec2(  w, 0.0)).rgb);
	a_pixel[5] = Intensity(texture(a_texture, a_uv + vec2( -w,  -h)).rgb);
	a_pixel[6] = Intensity(texture(a_texture, a_uv + vec2(0.0,  -h)).rgb);
	a_pixel[7] = Intensity(texture(a_texture, a_uv + vec2(  w,  -h)).rgb);
}

void main()
{
	float pixel[8];

	GetTexelKernel(pixel, vUV, u_texture);
	vec3 rgb = texture(u_texture, vUV).rgb;
	float sobel_edge_v = pixel[5] + (2.0 * pixel[6]) + pixel[7] - (pixel[0] + (2.0 * pixel[1]) + pixel[2]);
	float sobel_edge_h = pixel[2] + (2.0 * pixel[4]) + pixel[7] - (pixel[0] + (2.0 * pixel[3]) + pixel[5]);
	float sobelValue = sqrt((sobel_edge_v * sobel_edge_v) + (sobel_edge_h * sobel_edge_h));
	
	sobelValue = step(0.6, sobelValue);
	vec3 red = vec3(1.0, 0.0, 0.0);
	fragColour = vec4(rgb + (red * sobelValue), 1.0);
}