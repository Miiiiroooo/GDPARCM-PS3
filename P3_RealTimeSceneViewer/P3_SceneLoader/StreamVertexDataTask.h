#pragma once
#include <string>
#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"
#include "multithreading/tasks/AWorkerTask.h"

class StreamVertexDataTask : public AWorkerTask
{
public:
	StreamVertexDataTask(int startingIndex, int max, std::string modelName, std::vector<float> data, grpc::ServerWriter<ModelData>* writer);
	~StreamVertexDataTask();

	void ExecuteTask() override; 


private:
	int startingIndex;
	int max;
	std::string modelName;
	std::vector<float> data;
	grpc::ServerWriter<ModelData>* writer;
};