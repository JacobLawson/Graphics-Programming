#ifndef __PARTICLE_EMITTER_H_
#define __PARTICLE_EMITTER_H_

#include <glm/glm.hpp>
#include <string>

struct Particle
{
	float Type;
	glm::vec4 Pos;
	glm::vec4 Vel;
	float Age;
};

class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();

	void InitialiseParticles();	
	void LoadParticleTextures(std::string a_randomPos,  std::string a_texture);

	void Draw(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix);

	//getters and setters
	void SetShader(unsigned int a_shaderID, unsigned int a_tfShaderID) { m_particle_ProgramID = a_shaderID; m_particle_tfProgram = a_tfShaderID; }
	unsigned int GetShaderProgram() { return m_particle_ProgramID; }
	unsigned int GetTFShaderProgram() { return m_particle_tfProgram; }

	unsigned int GetParticleCount() { return m_particleCount; }

private:
	//transform feedback 
	unsigned int m_particle_ProgramID;
	unsigned int m_particle_tfProgram;

	unsigned int m_tf[2];
	unsigned int m_vb[2];
	unsigned int m_tf_curr;
	unsigned int m_tf_prev;
	unsigned int m_tfVAO[2];

	//Texture Handle for OpenGL
	unsigned int m_textureID;
	unsigned int m_randomTexture;
	////use atomic int to track particles
	unsigned int m_atomicBuffer;
	unsigned int m_acReadBuffer;

	//counter for particles
	unsigned int m_particleCount;

};

#endif //__PARTICLE_EMITTER_H_
