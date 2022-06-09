#version 150

in float particleType;
in vec4 particlePos;
in vec4 particleVelocity;
in float particleAge;

//We can output a structure from a shader as well
//we do not need to only output basic native types
out Vertex
{
	vec4 Colour;
} vertex;

//This is a simple pass through shader that simply emitts the vertex position
//There is not model or view space transform carried out here either
void main() 
{ 
	vertex.Colour = vec4(0.8 - particleVelocity.x,0.5 -  particleVelocity.z,0.0, 0.9);
	gl_Position = particlePos;
}