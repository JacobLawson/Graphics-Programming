#ifndef __Camera_H_
#define __Camera_H_

#include <glm/glm.hpp>

class Camera
{
public:
	Camera();
	~Camera();

	void Update(float a_deltaTime);

	//getters and setters
	void setMatrix(glm::mat4 a_cameraMatrix) { m_cameraMatrix = a_cameraMatrix; }
	glm::mat4 GetMatrix() { return m_cameraMatrix; }

protected:
	glm::mat4 m_cameraMatrix;
};

#endif //__Camera_H_