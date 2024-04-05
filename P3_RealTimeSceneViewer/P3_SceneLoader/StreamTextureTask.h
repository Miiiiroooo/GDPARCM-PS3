#pragma once
#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"
#include "multithreading/tasks/AWorkerTask.h"
#include "ServerSemaphore.h"

class StreamTextureTask : public AWorkerTask
{
public:
	StreamTextureTask(int sceneID, int maxBatchSize, int maxThreads, std::string textureName, std::string texturePath, grpc::ServerWriter<TextureData>* writer);
	~StreamTextureTask();

	void ExecuteTask() override;


private:
	int sceneID;
	int maxBatchSize;
	int maxThreads;
	std::string textureName;
	std::string texturePath;
	std::binary_semaphore* guard;
	std::binary_semaphore* finishedSem; 
	grpc::ServerWriter<TextureData>* writer;
};