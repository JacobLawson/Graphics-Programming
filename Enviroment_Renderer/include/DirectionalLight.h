#ifndef __DIRECTIONAL_LIGHT_H_
#define __DIRECTIONAL_LIGHT_H_

#include <glm/glm.hpp>

class DirectionalLight
{
public:
	DirectionalLight();
	~DirectionalLight();

	void Update(float a_deltaTime);

	void SetPosition(glm::vec4 a_position) { m_position = a_position; }
	glm::vec4 GetPosition() { return m_position; }

protected:
	glm::vec4 m_position;
};

#endif	//__DIRECTIONAL_LIGHT_H_