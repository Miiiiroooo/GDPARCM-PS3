#include "PerspectiveCameraObject.h"

PerspectiveCameraObject::PerspectiveCameraObject() : CameraObject::CameraObject()
{
	SetProjection(800, 800);
}

void PerspectiveCameraObject::SetProjection(float height, float width)
{
	this->perspectiveProjection = glm::perspective(glm::radians(90.0f), (height / width), 0.1f, 500.0f);
}

void PerspectiveCameraObject::ApplyProjection(GLuint ShaderProgram)
{
	unsigned int projLoc = glGetUniformLocation(ShaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveProjection));
}

void PerspectiveCameraObject::UpdateViewMatrix(glm::mat4 targetTransform)
{
	glm::vec3 target;
	target.x = targetTransform[3][0];
	target.y = targetTransform[3][1];
	target.z = targetTransform[3][2];


	F = glm::normalize(glm::vec3(cameraPos - target));
	R = glm::normalize(glm::cross(worldUp, F));
	U = glm::cross(F, R);

	viewMatrix = glm::lookAt(cameraPos, target + F, worldUp);
}

glm::mat4 PerspectiveCameraObject::GetPerspectiveProjection()
{
	return this->perspectiveProjection;
}
