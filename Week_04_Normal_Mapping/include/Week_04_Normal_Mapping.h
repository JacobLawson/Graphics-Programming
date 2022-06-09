#ifndef __Week_04_Normal_Mapping_H_
#define __Week_04_Normal_Mapping_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>
#include <string>

// Derived application class that wraps up all globals neatly
class Week_04_Normal_Mapping : public Application
{
public:

	Week_04_Normal_Mapping();
	virtual ~Week_04_Normal_Mapping();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	bool LoadImageFromFile(std::string a_filePath, unsigned int& a_textureID);

	FBXFile* m_fbxModel;

	//shader program ID's
	unsigned int m_programID;
	unsigned int m_vertexShader;
	unsigned int m_fragmentShader;

	//vertex array objects
	unsigned int m_vao;
	unsigned int m_modelBuffer[2];

	//Texture Handle for OpenGL
	unsigned int m_textureID[2];

	//Vertex array object vbo, ibo
	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_modelMatrix;

	glm::vec4 m_lightPosition;
};


#endif // __Week_04_Normal_Mapping_H_