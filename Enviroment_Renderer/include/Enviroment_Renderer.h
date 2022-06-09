#ifndef __ENVIROMENT_RENDERER_H_
#define __ENVIROMENT_RENDERER_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <string>

class Enviroment_Renderer : public Application
{
public:
	Enviroment_Renderer();
	virtual ~Enviroment_Renderer();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	bool LoadImageFromFile(std::string a_filePath, unsigned int& a_textureID);

	struct SVertex
	{
		glm::vec4 pos;
		glm::vec4 colour;
	};

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int	m_programID;
	unsigned int	m_vertexShader;
	unsigned int	m_geometryShader;
	unsigned int	m_fragmentShader;

	unsigned int m_maxIndices;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	//transform feedback 
	unsigned int m_tfProgram;
	unsigned int m_tfVertexShader;
	unsigned int m_tfGeometryShader;

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
	unsigned int m_particleCount;
	SVertex* m_vertices;

	int		m_currentState;
};

#endif //__ENVIROMENT_RENDERER_H_