#pragma once
#include "CameraObject.h"
#include<unordered_map>

class PerspectiveCameraObject : public CameraObject
{
public:
	PerspectiveCameraObject();

	void SetProjection(float height, float width);
	void ApplyProjection(GLuint ShaderProgram);
	void UpdateViewMatrix(glm::mat4 targetTransform);

	void CameraMovement(unordered_map<string, bool> input, glm::vec2 mousePos, float deltaTime);
	glm::mat4 GetPerspectiveProjection();
	glm::mat4 perspectiveProjection;

private:

	float MOVEMENT_SPEED = 5.f;

};

