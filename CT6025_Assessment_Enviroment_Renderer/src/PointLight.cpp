#include "PointLight.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>


PointLight::PointLight()
{
	m_lightPos = glm::vec4(-0.67f, 0.6f, -1.47f, 1.f);
	m_lightDiffuse = glm::vec4(1.0, 1.0, 1.0, 0.f);
	m_lightAmbient = glm::vec4(0.125, 0.125, 0.125, 0.f);
	m_lightSpecular = glm::vec4(1.0, 1.0, 1.0, 0.f);
	m_lightSpecularPower = 10.0f;

	m_constant = 1.f;
	m_linear = 0.00f;
	m_quadratic = 0.032f;
}

PointLight::~PointLight()
{
}

unsigned int PointLight::GetConstantUniform(unsigned int a_shaderProgramID)
{
	unsigned int constantUniformLocation = glGetUniformLocation(a_shaderProgramID, "uLightConstant");
	glUniform1f(constantUniformLocation, m_constant);
	return constantUniformLocation;
}

unsigned int PointLight::GetLinearUniform(unsigned int a_shaderProgramID)
{
	unsigned int linearUniformLocation = glGetUniformLocation(a_shaderProgramID, "uLightLinear");
	glUniform1f(linearUniformLocation, m_linear);
	return linearUniformLocation;
}

unsigned int PointLight::GetQuadraticUniform(unsigned int a_shaderProgramID)
{
	unsigned int linearUniformLocation = glGetUniformLocation(a_shaderProgramID, "uLightQuadratic");
	glUniform1f(linearUniformLocation, m_quadratic);
	return linearUniformLocation;
}

unsigned int PointLight::GetLightSpecularUniform(unsigned int a_shaderProgramID, char* a_uniform)
{
	int lightSpecularUniformLoc = glGetUniformLocation(a_shaderProgramID, a_uniform);
	glUniform4fv(lightSpecularUniformLoc, 1, glm::value_ptr(glm::vec4(m_lightSpecular, m_lightSpecularPower)));
	return lightSpecularUniformLoc;
}
