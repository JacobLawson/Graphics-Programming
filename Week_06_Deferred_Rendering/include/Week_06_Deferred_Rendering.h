#ifndef __Shadows_H_
#define __Shadows_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "FBXFile.h"

// Derived application class that wraps up all globals neatly
class Week_06_Deferred_Rendering : public Application
{
public:

	Week_06_Deferred_Rendering();
	virtual ~Week_06_Deferred_Rendering();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	void CreateFrameBufferObject(unsigned int a_fbo, unsigned int a_diffuse, unsigned int a_normal, unsigned int a_worldPos, unsigned int a_depth);

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_modelMatrix;

	unsigned int m_geomPassProgramID;
	unsigned int m_fboProgramID;
	unsigned int m_fsProgramID;

	unsigned int m_vao[2];
	unsigned int m_vbo[2];
	unsigned int m_ibo[2];

	unsigned int m_fbo;
	unsigned int m_fbo_depth;
	unsigned int m_fbo_diffuse;
	unsigned int m_fbo_normal;
	unsigned int m_fbo_worldPosition;

	//FBX Model File
	FBXFile* m_fbxModel;

	glm::vec4 m_lightPos;
};

#endif // __Application_H_