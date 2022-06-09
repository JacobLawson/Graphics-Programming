#ifndef __Week_02_Fullscreen_Fragment_Shader_H_
#define __Week_02_Fullscreen_Fragment_Shader_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Week_02_Fullscreen_Fragment_Shader : public Application
{
public:

	Week_02_Fullscreen_Fragment_Shader();
	virtual ~Week_02_Fullscreen_Fragment_Shader();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	typedef struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	}Vertex;

	Vertex* m_verticies;

	unsigned int m_programID;
	unsigned int m_vertexShader;
	unsigned int m_fragmentShader;

	unsigned int m_maxVerts;
	unsigned int m_vao;				//Vertex array object
	unsigned int m_vbo;				//Vertex buffer object
	unsigned int m_ibo;				//index buffer object
};

#endif // __Week_02_Fullscreen_Fragment_Shader_H_