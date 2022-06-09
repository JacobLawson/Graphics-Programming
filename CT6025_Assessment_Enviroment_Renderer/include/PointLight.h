#ifndef __POINT_LIGHT_H_
#define __POINT_LIGHT_H_

#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();
	~PointLight();

	unsigned int GetConstantUniform(unsigned int a_shaderProgramID);
	unsigned int GetLinearUniform(unsigned int a_shaderProgramID);
	unsigned int GetQuadraticUniform(unsigned int a_shaderProgramID);

	unsigned int GetLightSpecularUniform(unsigned int a_shaderProgramID, char* a_uniform);

	float m_constant;
	float m_linear;
	float m_quadratic;

	float m_lightSpecularPower;

protected:

};

#endif // !__POINT_LIGHT_H_
