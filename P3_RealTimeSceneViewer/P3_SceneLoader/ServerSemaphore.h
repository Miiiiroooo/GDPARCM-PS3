#pragma once
#include <semaphore>

class ServerSemaphore
{
public:
	static std::binary_semaphore jsonFileSem;
	static std::binary_semaphore sceneProgressSem;


private:
	ServerSemaphore() {};
	ServerSemaphore(const ServerSemaphore&) {};
	ServerSemaphore& operator=(const ServerSemaphore&) {};
};