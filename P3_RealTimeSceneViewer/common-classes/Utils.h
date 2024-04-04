#pragma once
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Utils
{
public:
	template<typename MainClass, typename ToCheck>
	static bool InstanceOf(const MainClass* ptr);

	template<typename MainClass, typename ToConvert>
	static ToConvert* ConvertTo(MainClass* ptr);

	static int GetRandomInt(int min, int max);
	static float GetRandomFloat(float min, float max);


public:
	static const glm::vec3 WORLD_FORWARD;
	static const glm::vec3 WORLD_UP;
	static const glm::vec3 WORLD_RIGHT;
	static const glm::mat3 IDENTITY_MAT3;
	static const glm::mat4 IDENTITY_MAT4;
};


template<typename MainClass, typename ToCheck>
bool Utils::InstanceOf(const MainClass* ptr) {
	return dynamic_cast<const ToCheck*>(ptr) != nullptr;
}

template<typename MainClass, typename ToConvert>
ToConvert* Utils::ConvertTo(MainClass* ptr) {
	return dynamic_cast<ToConvert*>(ptr);
}