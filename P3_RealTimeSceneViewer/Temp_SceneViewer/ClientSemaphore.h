#pragma once
#include <semaphore>

class ClientSemaphore
{

public:
	static std::binary_semaphore sceneProgressSem1;
	static std::binary_semaphore sceneProgressSem2;
	static std::binary_semaphore sceneProgressSem3;
	static std::binary_semaphore sceneProgressSem4;
	static std::binary_semaphore sceneProgressSem5;


private:
	ClientSemaphore() {};
	ClientSemaphore(const ClientSemaphore&) {};
	ClientSemaphore& operator=(const ClientSemaphore&) {};
};