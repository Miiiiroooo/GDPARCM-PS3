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
	/*glm::vec3 target;
	target.x = targetTransform[3][0];
	target.y = targetTransform[3][1];
	target.z = targetTransform[3][2];


	F = glm::normalize(glm::vec3(cameraPos - target));
	R = glm::normalize(glm::cross(worldUp, F));
	U = glm::cross(F, R);

	viewMatrix = glm::lookAt(cameraPos, target + F, worldUp);*/
}

void PerspectiveCameraObject::CameraMovement(unordered_map<string, bool> input, glm::vec2 mousePos, float deltaTime)
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(mousePos.x)) * cos(glm::radians(mousePos.y));
	direction.y = sin(glm::radians(mousePos.y));
	direction.z = sin(glm::radians(mousePos.x)) * cos(glm::radians(mousePos.y));
	cameraFront = glm::normalize(direction);
	cameraRight = glm::normalize(glm::cross(cameraUp, cameraFront));

	if (input["W"] == true)
	{
		//viewMatrix = glm::translate(viewMatrix, glm::vec3(0, 1 * MOVEMENT_SPEED * deltaTime, 0));
		//cameraPos.z += -1 * MOVEMENT_SPEED * deltaTime;
		cameraPos += cameraFront * 1.0f * MOVEMENT_SPEED * deltaTime;
	}

	if (input["S"] == true)
	{
		cameraPos += cameraFront * -1.0f * MOVEMENT_SPEED * deltaTime;
	}

	if (input["A"] == true)
	{
		cameraPos += cameraRight * 1.0f * MOVEMENT_SPEED * deltaTime;
	}

	if (input["D"] == true)
	{
		cameraPos += cameraRight * -1.0f * MOVEMENT_SPEED * deltaTime;
	}

	if (input["Space"] == true)
	{
		cameraPos.y += 1 * MOVEMENT_SPEED * deltaTime;
	}

	if (input["LShift"] == true)
	{
		cameraPos.y += -1 * MOVEMENT_SPEED * deltaTime;
	}

	

	viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 PerspectiveCameraObject::GetPerspectiveProjection()
{
	return this->perspectiveProjection;
}
