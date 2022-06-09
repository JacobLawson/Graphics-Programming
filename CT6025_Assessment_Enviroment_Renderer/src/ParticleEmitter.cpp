#include "ParticleEmitter.h"

#include "Utilities.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

ParticleEmitter::ParticleEmitter()
{
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::InitialiseParticles()
{
	m_tf_curr = 0;
	m_tf_prev = 1;

	Particle launcher;
	launcher.Type = 0.f;
	launcher.Pos = glm::vec4(0.f, 0.f, 0.f, 1.f);
	launcher.Vel = glm::vec4(0.f, 1.f, 0.f, 0.f);
	launcher.Age = 0.f;

	//tx feedback buffers
	glGenTransformFeedbacks(2, m_tf);
	glGenVertexArrays(2, m_tfVAO);
	glGenBuffers(2, m_vb);
	srand(time(nullptr));
	for (int i = 0; i < 2; ++i)
	{
		glBindVertexArray(m_tfVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vb[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * 5, nullptr, GL_DYNAMIC_DRAW);
		for (int j = 0; j < 10; ++j)
		{
			glm::vec4 rotVel = glm::vec4(glm::rotate(glm::vec3(0.f, 1.f, 0.f), j * 10.f, glm::vec3(1.f, 0.f, 0.f)), 0.f);
			launcher.Vel = rotVel;
			glBufferSubData(GL_ARRAY_BUFFER, j * sizeof(Particle), sizeof(Particle), &launcher);
		}

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)20);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)36);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_tf[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vb[i]);

	}

	m_particleCount = 0;
	//atomic counter to track particles
	glGenBuffers(1, &m_atomicBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &m_particleCount, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	glGenBuffers(1, &m_acReadBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, m_acReadBuffer);
	glBufferData(GL_COPY_WRITE_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_READ);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

	glBindVertexArray(0);
}

void ParticleEmitter::LoadParticleTextures(std::string a_randomPos, std::string a_texture)
{
	Utility::LoadImageFromFile(a_randomPos, m_randomTexture);
	Utility::LoadImageFromFile(a_texture, m_textureID);
}

void ParticleEmitter::Draw(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix)
{
	glUseProgram(m_particle_tfProgram);
	//no frag shader so disable rasterisation
	glEnable(GL_RASTERIZER_DISCARD);
	unsigned int deltaT = glGetUniformLocation(m_particle_tfProgram, "deltaTime");
	glUniform1f(deltaT, Utility::getDeltaTime());
	unsigned int totalT = glGetUniformLocation(m_particle_tfProgram, "totalTime");
	glUniform1f(totalT, Utility::getTotalTime());
	unsigned int randTexUniformID = glGetUniformLocation(m_particle_tfProgram, "randomTexture");
	glUniform1i(randTexUniformID, 0);
	//set active texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_randomTexture);

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_atomicBuffer);
	glBindVertexArray(m_tfVAO[m_tf_curr]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_tf[m_tf_prev]);

	glBeginTransformFeedback(GL_POINTS);

	static bool firstDraw = true;
	if (firstDraw)
	{
		glDrawArrays(GL_POINTS, 0, 10);
		firstDraw = false;
	}
	else
	{
		glDrawTransformFeedback(GL_POINTS, m_tf[m_tf_curr]);
	}
	glEndTransformFeedback();
	glBindVertexArray(0);
	//turn rasterisation back on
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_atomicBuffer);

	//read counter through temporary buyffer and reset counter
	glBindBuffer(GL_COPY_WRITE_BUFFER, m_acReadBuffer);
	glCopyBufferSubData(GL_ATOMIC_COUNTER_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(GLuint));
	unsigned int* counterValue = (unsigned int*)glMapBufferRange(GL_COPY_WRITE_BUFFER, 0,
		sizeof(GLuint), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	m_particleCount = counterValue[0];
	counterValue[0] = 0;
	glUnmapBuffer(GL_COPY_WRITE_BUFFER);
	glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ATOMIC_COUNTER_BUFFER, 0, 0, sizeof(GLuint));
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	glUseProgram(0);

	//bing our shader program
	glUseProgram(m_particle_ProgramID);

	//get our shaders uniform location for our projectionViewMatrix and then use glUniformMatrix4fv to fill it with the correct data
	unsigned int projectionViewUniform = glGetUniformLocation(m_particle_ProgramID, "ProjectionView");
	glm::mat4 viewMatrix = glm::inverse(a_cameraMatrix);
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projectionViewMatrix * viewMatrix));

	unsigned int cameraPosUniform = glGetUniformLocation(m_particle_ProgramID, "cameraPosition");
	glUniform4fv(cameraPosUniform, 1, glm::value_ptr(a_cameraMatrix[3]));

	//bind our textureLocation variable from the shaders and set it's value to 0 as the active texture is texture 0
	unsigned int texUniformID = glGetUniformLocation(m_particle_ProgramID, "textureLoc");
	glUniform1i(texUniformID, 0);
	//set our active texture, and bind our loaded texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//bind our vertex array object
	glBindVertexArray(m_tfVAO[m_tf_prev]);

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_COLOR, GL_SRC_COLOR);
	glBlendEquation(GL_MAX);

	//depending on the state call draw with glDrawElements to draw our buffer
	//glDrawElements uses the index array in our index buffer to draw the vertices in our vertex buffer
	glDrawTransformFeedback(GL_POINTS, m_tf[m_tf_prev]);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glBindVertexArray(0);
	glUseProgram(0);

	m_tf_curr = m_tf_prev;
	m_tf_prev = (m_tf_prev + 1) & 0x1;
}
