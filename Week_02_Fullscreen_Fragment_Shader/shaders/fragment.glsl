#version 330

in vec4 vColour;

out vec4 fragColour;

//use a uniform to get cpu side data into the shader

uniform float u_time;
uniform vec2 u_resolution;

const float PI = 3.14159265;

float Circle(in vec2 a_st, in float a_radius)
{
	vec2 pos = vec2(0.5) - a_st;
	a_radius *= 0.6;
	return 1.0 - smoothstep(a_radius - (a_radius * 0.05), a_radius + (a_radius * 0.05), dot(pos, pos) * PI);

}

void main()
{
	vec2 uv = gl_FragCoord.xy / u_resolution;
	//fragColour = max(0.f, sin(u_time)) * vColour;

	uv *= 24.0;
	uv = fract(uv);
	float s = sin(u_time);
	vec4 tColour = vColour;
	if (s < 0)
	{
		tColour = tColour.gbra;
	}
	fragColour = mix(vec4(0.f, 0.f, 0.f, 1.f), tColour, Circle(uv, abs(sin(s))));
}