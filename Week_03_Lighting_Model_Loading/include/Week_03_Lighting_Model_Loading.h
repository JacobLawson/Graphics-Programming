#ifndef __Week_03_Lighting_Model_Loading_H_
#define __Week_03_Lighting_Model_Loading_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>

// Derived application class that wraps up all globals neatly
class Week_03_Lighting_Model_Loading : public Application
{
public:

	Week_03_Lighting_Model_Loading();
	virtual ~Week_03_Lighting_Model_Loading();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	FBXFile* m_fbxModel;

	//shader program ID's
	unsigned int m_programID;
	unsigned int m_vertexShader;
	unsigned int m_fragmentShader;

	//vertex array objects
	unsigned int m_vao;
	unsigned int m_modelBuffer[2];

	//Vertex array object vbo, ibo

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_modelMatrix;

	glm::vec4 m_lightPosition;
};

#endif // __Week_03_Lighting_Model_Loading_H_