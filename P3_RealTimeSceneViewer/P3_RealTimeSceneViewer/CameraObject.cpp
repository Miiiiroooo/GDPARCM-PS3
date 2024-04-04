#include "CameraObject.h"

CameraObject::CameraObject()
{
}

glm::vec3 CameraObject::GetCameraPosition()
{
	return this->cameraPos;
}

glm::mat4 CameraObject::GetViewMatrix()
{
	return viewMatrix;
}

glm::vec3 CameraObject::GetFront()
{
	return this->F;
}

void CameraObject::ApplyCameraPosition(GLuint ShaderProgram)
{
	GLuint cameraPosAddress = glGetUniformLocation(ShaderProgram, "cameraPos");
	glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));
}

void CameraObject::ApplyView(GLuint ShaderProgram)
{
	unsigned int viewLoc = glGetUniformLocation(ShaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}
