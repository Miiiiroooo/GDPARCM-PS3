#include "ServerSemaphore.h"


std::binary_semaphore ServerSemaphore::jsonFileSem(1);
std::binary_semaphore ServerSemaphore::sceneProgressSem(1);

std::binary_semaphore ServerSemaphore::finishedStreamTextureSem1(1);
std::binary_semaphore ServerSemaphore::finishedStreamTextureSem2(1);
std::binary_semaphore ServerSemaphore::finishedStreamTextureSem3(1);
std::binary_semaphore ServerSemaphore::finishedStreamTextureSem4(1);
std::binary_semaphore ServerSemaphore::finishedStreamTextureSem5(1);
std::binary_semaphore ServerSemaphore::streamTextureGuardSem1(1);
std::binary_semaphore ServerSemaphore::streamTextureGuardSem2(1);
std::binary_semaphore ServerSemaphore::streamTextureGuardSem3(1);
std::binary_semaphore ServerSemaphore::streamTextureGuardSem4(1);
std::binary_semaphore ServerSemaphore::streamTextureGuardSem5(1);
std::unordered_map<int, int> ServerSemaphore::finishedStreamTextureMap;