#include "Camera.h"
#include "Utilities.h"
#pragma once

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Update(float a_deltaTime)
{
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);
}