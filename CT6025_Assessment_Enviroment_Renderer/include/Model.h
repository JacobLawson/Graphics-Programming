#ifndef __MODEL_H_
#define __MODEL_H_

#include <string>
#include <glm/glm.hpp>
#include "FBXFile.h"

class DirectionalLight;
class PointLight;
class SpotLight;

class Model
{
public:
	Model();
	~Model();

	void Load(const char* a_modelFile);
	void Initialise(unsigned int a_windowWidth, unsigned int a_windowHeight, unsigned int a_shadowFramebufferSize);
	void SetupFrameBuffer(unsigned int& a_fbo, unsigned int a_targetWidth, unsigned int a_targetHeight, unsigned int& a_renderTexture, unsigned int& a_depthTexture);
	void Update(float a_deltaTime, glm::mat4 a_depthViewMatrix, glm::mat4 a_depthProjectionMatrix);
	void Draw(glm::mat4 a_projectionViewMatrix, glm::mat4 a_cameraMatrix, DirectionalLight* a_directionalLight, PointLight* a_pointLight, SpotLight* a_spotLight, unsigned int a_shaderProgramID);

	//getters setters
	void SetShader(unsigned int a_shaderID, unsigned int a_Shadow_ShaderID) { m_modelShader = a_shaderID; m_shadowShader = a_Shadow_ShaderID; }
	unsigned int GetShaderProgram() { return m_modelShader; }
	unsigned int GetShadowShaderProgram() { return m_shadowShader; }

	FBXFile* GetModelData() { return m_fbxModel; }
	unsigned int GetModelFBO() { return m_FBO; }

	unsigned int GetFBOTexture() { return m_FBO_texture; }
	unsigned int GetLinearDepthTexture() { return m_FBO_Linear_Depth; }

private:

	unsigned int m_modelShader;
	unsigned int m_shadowShader;

	FBXFile* m_fbxModel;
	glm::mat4 m_modelMatrix;
	glm::mat4 m_shadowProjectionMatrix;

	//model data
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	//frame buffer variables
	unsigned int m_FBO;
	unsigned int m_FBO_texture;
	unsigned int m_FBO_depth_texture;
	unsigned int m_FBO_Linear_Depth;
};

#endif //__MODEL_H_