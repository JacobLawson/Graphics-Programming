#ifndef __QUAD_H_
#define __QUAD_H_

#include <glm/glm.hpp>

typedef struct Vertex
{
	glm::vec4 position;
	glm::vec4 colour;
}Vertex;

class Quad
{
public:
	Quad();
	~Quad();

	bool Draw(unsigned int a_texture);

	void SetShader(unsigned int a_shaderID) { m_ShaderProgramID = a_shaderID; }
	unsigned int GetShaderProgram() { return m_ShaderProgramID; }

	unsigned int GetVAO() { return m_vao; }
	unsigned int GetVBO() { return m_vbo; }
	unsigned int GetIBO() { return m_ibo; }

protected:

	Vertex* m_verticies;

	unsigned int m_ShaderProgramID;

	unsigned int m_vbo;
	unsigned int m_vao;
	unsigned int m_ibo;
};

#endif //__QUAD_H_