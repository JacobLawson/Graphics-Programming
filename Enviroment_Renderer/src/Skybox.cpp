#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>


#include "Texture.h"
#include "Skybox.h"

SkyBox::SkyBox()
{
}

SkyBox::~SkyBox()
{
	glDeleteBuffers(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);

	glDeleteProgram(m_skyboxShader);
	delete m_texture;
	m_texture = nullptr;
}

bool SkyBox::Draw(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix)
{
	//Draw the skybox
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

bool SkyBox::LoadSkyBox(std::vector<std::string> a_textureFaces, unsigned int a_cubeMapImageTags[6])
{
	//Texture* m_texture = new Texture();
	//m_cubeMapTextureID = m_texture->LoadCubeMap(a_textureFaces, a_cubeMapImageTags);
	return true;
}

bool SkyBox::GenerateSkyBoxBuffers()
{
	//create an array of the verts used
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

	return true;
}
