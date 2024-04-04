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


	glm::mat4 GetPerspectiveProjection();
	glm::mat4 perspectiveProjection;

private:

	glm::vec3 offset;
	glm::mat4 pivot = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	float rotationSpeed = 100.f;
};

