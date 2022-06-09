#include "..\..\CT6025_Assessment_Enviroment_Renderer\include\Camera.h"
#include "..\include\Camera.h"
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

bool Camera::Update(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);
	return true;
}
