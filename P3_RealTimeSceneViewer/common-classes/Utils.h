#pragma once
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Utils
{
public:
	static const glm::vec3 WORLD_FORWARD;
	static const glm::vec3 WORLD_UP;
	static const glm::vec3 WORLD_RIGHT;
	static const glm::mat3 IDENTITY_MAT3;
	static const glm::mat4 IDENTITY_MAT4;
};