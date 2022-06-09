#include "SpotLight.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>


SpotLight::SpotLight()
{
	m_lightPos = glm::vec4(-0.67f, 0.6f, -1.47f, 1.f);
	m_lightDir = glm::vec4(0.f, -1.f, 0.f, 0.f);
	m_lightDiffuse = glm::vec4(1.0, 1.0, 1.0, 0.f);
	m_lightAmbient = glm::vec4(0.125, 0.125, 0.125, 0.f);
	m_lightSpecular = glm::vec4(1.0, 1.0, 1.0, 0.f);
	m_lightSpecularPower = 10.0f;

	m_angle = 42.0f;

	m_constant = 1.0f;
	m_linear = 0.0f;
	m_quadratic = 0.05f;

	m_lightMoveDirection = 1;
}

SpotLight::~SpotLight()
{
}

void SpotLight::Update(float a_deltaTime)
{
}

unsigned int SpotLight::GetConstantUniform(unsigned int a_shaderProgramID)
{
	unsigned int constantUniformLocation = glGetUniformLocation(a_shaderProgramID, "uSpotLightConstant");
	glUniform1f(constantUniformLocation, m_constant);
	return constantUniformLocation;
}

unsigned int SpotLight::GetLinearUniform(unsigned int a_shaderProgramID)
{
	unsigned int linearUniformLocation = glGetUniformLocation(a_shaderProgramID, "uSpotLightLinear");
	glUniform1f(linearUniformLocation, m_linear);
	return linearUniformLocation;
}

unsigned int SpotLight::GetQuadraticUniform(unsigned int a_shaderProgramID)
{
	unsigned int linearUniformLocation = glGetUniformLocation(a_shaderProgramID, "uSpotLightQuadratic");
	glUniform1f(linearUniformLocation, m_quadratic);
	return linearUniformLocation;
}

unsigned int SpotLight::GetAngleUniform(unsigned int a_shaderProgramID)
{
	unsigned int angleUniformLocation = glGetUniformLocation(a_shaderProgramID, "uSpotLightAngle");
	glUniform1f(angleUniformLocation, m_angle);
	return angleUniformLocation;
}

unsigned int SpotLight::GetLightSpecularUniform(unsigned int a_shaderProgramID, char* a_uniform)
{
	int lightSpecularUniformLoc = glGetUniformLocation(a_shaderProgramID, a_uniform);
	glUniform4fv(lightSpecularUniformLoc, 1, glm::value_ptr(glm::vec4(m_lightSpecular, m_lightSpecularPower)));
	return lightSpecularUniformLoc;
}

unsigned int SpotLight::GetLightDirUniform(unsigned int a_shaderProgramID)
{
	
	m_lightDir.w = 0.f;
	m_lightDir = glm::normalize(m_lightDir);
	unsigned int lightDirUniform = glGetUniformLocation(a_shaderProgramID, "uSpotLightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(m_lightDir));

	return lightDirUniform;
}
