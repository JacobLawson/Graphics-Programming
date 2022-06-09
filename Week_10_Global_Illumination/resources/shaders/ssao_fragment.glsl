#version 150

in vec2 UV;
out float FragColor;

uniform sampler2D ViewPosTexture;
uniform sampler2D NormalTexture;
uniform sampler2D RandomMapTexture;

//use a constant to store our cross kernal information
const vec2 crossKernal[4] = vec2[](vec2(1, 0), vec2(-1, 0), vec2(0, 1), vec2(0, -1));

float CalculateOcclusion(in vec2 offset, in vec3 viewPosition, in vec3 viewNormal)
{
	//get the difference between the two fragment positions
	vec3 diff = texture(ViewPosTexture, UV + offset).rgb - viewPosition;
	vec3 v = normalize(diff);
	//scale the distance
	float d = length(diff);
	//calculate the occlusion max is used to reject negative dot prod vals
	return max(0.0, dot(viewNormal, v)) * (1.0 / (1.0 + d));
}

void main()
{
	//sample view space textures
	vec3 viewPosition = texture(ViewPosTexture, UV).rgb;
	vec3 viewNormal = texture(NormalTexture, UV).rgb * 2.0 - 1.0;

	//base the radius sample off the distance to the fragment
	float raduis = 1.0 / viewPosition.z;

	//perform 16 occlusion tests, accumullating occlusiuon values
	float ao = 0.0;
	int itterations = 4; //reduce itterations for higher performance

	for (int j = 0; j < itterations; ++j)
	{
		//scale offsets
		vec2 offset1 = crossKernal[j] * raduis;
		//rotate the offset
		vec2 offset2 = vec2(offset1.x * 0.707 - offset1.y * 0.707, offset1.x * 0.707 + offset1.y * 0.707);

		//use random cross kernal to sample occlusion
		ao += CalculateOcclusion(offset1 * 0.25, viewPosition, viewNormal);
		ao += CalculateOcclusion(offset2 * 0.50, viewPosition, viewNormal);
		ao += CalculateOcclusion(offset1 * 0.75, viewPosition, viewNormal);
		ao += CalculateOcclusion(offset2, viewPosition, viewNormal);
	}
	ao /= itterations;
	FragColor = 1.0 - ao;
}
