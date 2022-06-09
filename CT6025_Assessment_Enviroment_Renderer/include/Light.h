#ifndef __LIGHT_H_
#define __LIGHT_H_

#include <glm/glm.hpp>

class Light
{
public:
	Light();
	~Light();

	void VisualisePosition();
	virtual void Update();

	void SetLightPos(glm::vec4 a_lightPos) { m_lightPos = a_lightPos; }
	glm::vec4 GetLightPos() { return m_lightPos; }

	unsigned int GetLightPositionUniform(unsigned int a_shaderProgramID, char* a_uniform);
	unsigned int GetLightColourUniform(unsigned int a_shaderProgramID, char* a_uniform);
	unsigned int GetLightAmbientUniform(unsigned int a_shaderProgramID, char* a_uniform);
	//unsigned int GetLightSpecularUniform(unsigned int a_shaderProgramID, char* a_uniform);

	glm::vec4 m_lightAmbient;
	glm::vec4 m_lightDiffuse;
	glm::vec3 m_lightSpecular;

protected:
	glm::vec4 m_lightPos;
};

#endif // !__LIGHT_H_
