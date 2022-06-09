#include "..\include\DirectionalLight.h"
#include "Gizmos.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

DirectionalLight::DirectionalLight()
{
	m_lightDiffuse = glm::vec4(1.0, 1.0, 1.0, 0.f);
	m_lightAmbient = glm::vec4(0.125, 0.125, 0.125, 0.f);
	m_lightSpecular = glm::vec4(1.0, 1.0, 1.0, 0.f);
	m_lightSpecularPower = 32.0f;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Update(float a_deltaTime)
{
	// rotate point
	float s = sinf(a_deltaTime * 0.2f);
	float c = cosf(a_deltaTime * 0.2f);
	glm::vec4 temp = m_lightPos;
	m_lightPos.x = temp.x * c - temp.z * s;
	m_lightPos.z = temp.x * s + temp.z * c;
	m_lightPos.w = 0;
	m_lightPos = glm::normalize(m_lightPos) * 25.f;
}

unsigned int DirectionalLight::GetLightDirUniform(unsigned int a_shaderProgramID)
{
	glm::vec4 lightDir = GetLightPos();
	lightDir.w = 0.f;
	lightDir = glm::normalize(lightDir);
	unsigned int lightDirUniform = glGetUniformLocation(a_shaderProgramID, "uLightDirection");
	glUniform4fv(lightDirUniform, 1, glm::value_ptr(lightDir));

	return lightDirUniform;
}

unsigned int DirectionalLight::GetLightSpecularUniform(unsigned int a_shaderProgramID, char* a_uniform)
{
	int lightSpecularUniformLoc = glGetUniformLocation(a_shaderProgramID, a_uniform);
	glUniform4fv(lightSpecularUniformLoc, 1, glm::value_ptr(glm::vec4(m_lightSpecular, m_lightSpecularPower)));
	return lightSpecularUniformLoc;
}
