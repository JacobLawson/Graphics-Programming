#ifndef __CT6025_Assessment_Enviroment_Renderer_H_
#define __CT6025_Assessment_Enviroment_Renderer_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "FBXFile.h"
#include "SimpleVertex.h"

class Camera;
class DirectionalLight;
class PointLight;
class SpotLight;
class SkyBox;
class WaterQuad;
class ReflectionQuad;
class Model;
class ParticleEmitter;

// Derived application class that wraps up all globals neatly
class CT6025_Assessment_Enviroment_Renderer : public Application
{
public:

	CT6025_Assessment_Enviroment_Renderer();
	virtual ~CT6025_Assessment_Enviroment_Renderer();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	void DefferedRendering();
	void DrawDefferedRendering();

	void SetupShaders();

	void GUI_Lighting();
	void GUI_Tesselation();

	void UpdateGizmos();
	void DrawGizmos();
	
	Camera* m_mainCamera;
	
	DirectionalLight* m_directionalLight;
	PointLight* m_pointLight;
	SpotLight* m_spotLight;

	SkyBox* m_skyBox;
	WaterQuad* m_water;
	ReflectionQuad* m_reflection;
	Model* m_sceneModel;
	ParticleEmitter* m_EmberParticleEmitter;

	glm::mat4	m_projectionMatrix;
	glm::mat4	m_shadowProjectionMatrix;

	//shader programs
	unsigned int m_skyboxShader;
	unsigned int m_waterShader;
	unsigned int m_modelShader;
	unsigned int m_shadowShader;
	unsigned int m_particleProgramID;
	unsigned int m_particle_tfProgram;
	unsigned int m_reflection_programID;
	unsigned int m_reflectionFBO_programID;

	//Geometry Particle system

	//Tesselation Shader


	//other
	SVertex* m_vertices;
	bool m_bRenderWireFrame;
	int	m_currentState;

};

#endif // __CT6025_Assessment_Enviroment_Renderer_H_