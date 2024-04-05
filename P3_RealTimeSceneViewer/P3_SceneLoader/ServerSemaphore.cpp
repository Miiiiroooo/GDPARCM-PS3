#include "ServerSemaphore.h"


std::binary_semaphore ServerSemaphore::jsonFileSem(1);
std::binary_semaphore ServerSemaphore::sceneProgressSem(1);