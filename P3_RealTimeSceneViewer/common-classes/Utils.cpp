#include "Utils.h"

const glm::vec3 Utils::WORLD_FORWARD = glm::vec3(0.f, 0.f, 1.f);
const glm::vec3 Utils::WORLD_UP = glm::vec3(0.f, 1.f, 0.f);
const glm::vec3 Utils::WORLD_RIGHT = glm::vec3(1.f, 0.f, 0.f);
const glm::mat3 Utils::IDENTITY_MAT3 = glm::mat3(1.0f);
const glm::mat4 Utils::IDENTITY_MAT4 = glm::mat4(1.0f);


int Utils::GetRandomInt(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

float Utils::GetRandomFloat(float min, float max)
{
	return ((float)rand() / RAND_MAX) * (max - min) + min;
}

int Utils::GetRandomInt(int min, int max, std::mt19937* gen)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(*gen);
}

float Utils::GetRandomFloat(float min, float max, std::mt19937* gen)
{
	std::uniform_real_distribution<float> distribution(min, max); 
	return distribution(*gen);
}