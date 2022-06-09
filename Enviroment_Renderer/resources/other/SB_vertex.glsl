#version 150

in vec3 Position;

out vec3 TexCoords;

uniform mat4 u_projectionViewMatrix;

void main()
{
	TexCoords = Position;
	vec4 pos = u_projectionViewMatrix * vec4(Position * 1000.0f, 1.0f);
	gl_Position = pos.xyww;
}

