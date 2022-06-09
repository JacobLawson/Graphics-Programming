#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "Quad.h"

Quad::Quad()
{
	Vertex* verts = new Vertex[4];
	verts[0].position = glm::vec4(-1.f, -1.f, 0.f, 1.f);
	verts[1].position = glm::vec4(-1.f, 1.f, 0.f, 1.f);
	verts[2].position = glm::vec4(1.f, -1.f, 0.f, 1.f);
	verts[3].position = glm::vec4(1.f, 1.f, 0.f, 1.f);
	verts[0].colour = glm::vec4(1.f, 0.f, 0.f, 1.f);
	verts[1].colour = glm::vec4(0.f, 1.f, 0.f, 1.f);
	verts[2].colour = glm::vec4(0.f, 0.f, 1.f, 1.f);
	verts[3].colour = glm::vec4(1.f, 1.f, 1.f, 1.f);
	unsigned int indices[] = { 0, 2, 1, 3, 1, 2 };

	//generate and bind vertex array object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	//genearate and bind vertex buffer object
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)0 + 16);
	delete[] verts;
	//Generate and fill index buffer data
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//End Initialisation of Quad
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Quad::~Quad()
{
	glDeleteProgram(m_ShaderProgramID);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);
}

bool Quad::Draw(unsigned int a_texture)
{
	glUseProgram(m_ShaderProgramID);
	glBindVertexArray(m_vao);
	glBindTexture(GL_TEXTURE_2D, a_texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	return true;
}
