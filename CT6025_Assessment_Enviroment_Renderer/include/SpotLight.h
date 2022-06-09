#ifndef __SPOT_LIGHT_H_
#define __SPOT_LIGHT_H_

#include "Light.h"

class SpotLight : public Light
{
public:
	SpotLight();
	~SpotLight();

	virtual void Update(float a_deltaTime);

	unsigned int GetConstantUniform(unsigned int a_shaderProgramID);
	unsigned int GetLinearUniform(unsigned int a_shaderProgramID);
	unsigned int GetQuadraticUniform(unsigned int a_shaderProgramID);

	unsigned int GetAngleUniform(unsigned int a_shaderProgramID);
	unsigned int GetLightSpecularUniform(unsigned int a_shaderProgramID, char* a_uniform);

	unsigned int GetLightDirUniform(unsigned int a_shaderProgramID);

	int m_lightMoveDirection;
	glm::vec4 m_lightDir;

	float m_angle;

	float m_constant;
	float m_linear;
	float m_quadratic;

	float m_lightSpecularPower;

};

#endif // !__SPOT_LIGHT_H_
