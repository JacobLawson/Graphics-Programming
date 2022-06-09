#include "Light.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "Gizmos.h"


Light::Light()
{
	m_lightDiffuse = glm::vec4(1.0, 1.0, 1.0, 0.f);
	m_lightAmbient = glm::vec4(0.125, 0.125, 0.125, 0.f);
	m_lightSpecular = glm::vec4(1.0, 1.0, 1.0, 0.f);
}

Light::~Light()
{
}

void Light::Update()
{
}

void Light::VisualisePosition()
{
	Gizmos::addBox(m_lightPos.xyz, glm::vec3(0.2f, 0.2f, 0.2f), true, m_lightDiffuse);
}

unsigned int Light::GetLightPositionUniform(unsigned int a_shaderProgramID, char* a_uniform)
{
	int lightPositionUniformLoc = glGetUniformLocation(a_shaderProgramID, a_uniform);
	glUniform4fv(lightPositionUniformLoc, 1, glm::value_ptr(m_lightPos));
	return lightPositionUniformLoc;
}

unsigned int Light::GetLightColourUniform(unsigned int a_shaderProgramID, char* a_uniform)
{
	int lightColourUniformLoc = glGetUniformLocation(a_shaderProgramID, a_uniform);
	glUniform4fv(lightColourUniformLoc, 1, glm::value_ptr(m_lightDiffuse));
	return lightColourUniformLoc;
}

unsigned int Light::GetLightAmbientUniform(unsigned int a_shaderProgramID, char* a_uniform)
{
	int lightAmbientUniformLoc = glGetUniformLocation(a_shaderProgramID, a_uniform);
	glUniform4fv(lightAmbientUniformLoc, 1, glm::value_ptr(m_lightAmbient));
	return lightAmbientUniformLoc;
}
