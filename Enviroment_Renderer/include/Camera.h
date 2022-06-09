#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "Application.h"
#include <glm/glm.hpp>

class Camera
{
public:
	Camera();
	~Camera();

	bool Update(float a_deltaTime);

	//getters and setters
	glm::mat4 GetCameraMatrix() { return m_cameraMatrix; }
	void SetCameraMatrix(glm::mat4 a_newCameraMatrix) { m_cameraMatrix = a_newCameraMatrix; }

protected:
	glm::mat4	m_cameraMatrix;
};

#endif //__CAMERA_H_