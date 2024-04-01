#include "Utils.h"

const glm::vec3 Utils::WORLD_FORWARD = glm::vec3(0.f, 0.f, 1.f);
const glm::vec3 Utils::WORLD_UP = glm::vec3(0.f, 1.f, 0.f);
const glm::vec3 Utils::WORLD_RIGHT = glm::vec3(1.f, 0.f, 0.f);
const glm::mat3 Utils::IDENTITY_MAT3 = glm::mat3(1.0f);
const glm::mat4 Utils::IDENTITY_MAT4 = glm::mat4(1.0f);


template<typename MainClass, typename ToCheck>
inline bool InstanceOf(const MainClass* ptr) {
	return dynamic_cast<const ToCheck*>(ptr) != nullptr;
}

template<typename MainClass, typename ToConvert>
inline ToConvert* ConvertTo(MainClass* ptr) {
	return dynamic_cast<ToConvert*>(ptr);
}