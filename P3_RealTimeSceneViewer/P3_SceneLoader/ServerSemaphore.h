#pragma once
#include <semaphore>
#include <unordered_map>

class ServerSemaphore
{
public:
	static std::binary_semaphore jsonFileSem;
	static std::binary_semaphore sceneProgressSem;

	static std::binary_semaphore finishedStreamTextureSem1;
	static std::binary_semaphore finishedStreamTextureSem2;
	static std::binary_semaphore finishedStreamTextureSem3;
	static std::binary_semaphore finishedStreamTextureSem4;
	static std::binary_semaphore finishedStreamTextureSem5;
	static std::binary_semaphore streamTextureGuardSem1;
	static std::binary_semaphore streamTextureGuardSem2;
	static std::binary_semaphore streamTextureGuardSem3;
	static std::binary_semaphore streamTextureGuardSem4;
	static std::binary_semaphore streamTextureGuardSem5;
	static std::unordered_map<int, int> finishedStreamTextureMap;


private:
	ServerSemaphore() {};
	ServerSemaphore(const ServerSemaphore&) {};
	ServerSemaphore& operator=(const ServerSemaphore&) {};
};