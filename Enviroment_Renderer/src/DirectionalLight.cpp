#include "..\include\DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Update(float a_deltaTime)
{
	// rotate point
	float s = sinf(a_deltaTime * 0.2f);
	float c = cosf(a_deltaTime * 0.2f);
	glm::vec4 temp = m_position;
	m_position.x = temp.x * c - temp.z * s;
	m_position.z = temp.x * s + temp.z * c;
	m_position.w = 0;
	m_position = glm::normalize(m_position) * 25.f;
}
