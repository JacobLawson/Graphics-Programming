#ifndef __Week_01_VAO_H_
#define __Week_01_VAO_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Week_01_VAO : public Application
{
public:

	Week_01_VAO();
	virtual ~Week_01_VAO();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	};

	Vertex* m_verticies;
	
	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_programID;
	unsigned int m_vertexShader;
	unsigned int m_fragmentShader;

	unsigned int m_maxVerts;
	unsigned int m_vao;				//Vertex array object
	unsigned int m_vbo;				//Vertex buffer object
	unsigned int m_ibo;				//index buffer object
};

#endif // __Week_01_VAO_H_