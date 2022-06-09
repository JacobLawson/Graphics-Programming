#include "Skybox.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "Texture.h"

SkyBox::SkyBox()
{
	m_skyboxShader = -1;
}

SkyBox::~SkyBox()
{
	m_texture->unload();
	delete m_texture;
	m_texture = nullptr;

	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

bool SkyBox::Draw(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix)
{
	// Draw the skybox
	glDepthFunc(GL_LEQUAL);
	glUseProgram(m_skyboxShader);
	glDepthMask(GL_FALSE);

	glm::mat4 viewMatrix = glm::inverse(a_cameraMatrix);
	unsigned int projectionViewUniformLocation = glGetUniformLocation(m_skyboxShader, "u_projectionViewMatrix");
	//send this location a pointer to our glm::mat4 (send across float data)
	glUniformMatrix4fv(projectionViewUniformLocation, 1, false, glm::value_ptr(a_projectionViewMatrix * viewMatrix));

	glBindVertexArray(m_vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	glUseProgram(0);
	return true;
}

bool SkyBox::LoadSkyBox(std::vector<std::string> a_textureFaces)
{
	//setup the positioning of faces
	unsigned int cubemapImage_tag[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
										GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
										GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, };

	//Load Skybox images ast Cubemap Texture
	m_texture = new Texture;
	m_cubeMapTextureID = m_texture->LoadCubeMap(a_textureFaces, cubemapImage_tag);
	return true;
}

void SkyBox::GenerateSkyBoxBuffers()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//fill vertex buffer with Skybox data
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(float) * 108, skyboxVertices, 0);
	//Generate vertex array object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	//enale the vertex array state, since we're sending in an array of verticies
	glEnableVertexAttribArray(0);

	//Specify where our vertex array is, how many componets each vetex has,
	//the data type of each component and whether the data is normalised or not
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
