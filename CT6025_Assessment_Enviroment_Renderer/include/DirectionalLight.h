#ifndef __DIRECTIONAL_LIGHT_H_
#define __DIRECTIONAL_LIGHT_H_

#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	~DirectionalLight();

	virtual void Update(float a_deltaTime);

	void SetLightPos(glm::vec4 a_lightPos) { m_lightPos = a_lightPos; }
	glm::vec4 GetLightPos() { return m_lightPos; }

	unsigned int GetLightSpecularUniform(unsigned int a_shaderProgramID, char* a_uniform);

	unsigned int GetLightDirUniform(unsigned int a_shaderProgramID);
	float m_lightSpecularPower;

};

#endif // !__DIRECTIONAL_LIGHT_H_
