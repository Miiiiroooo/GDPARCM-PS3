#pragma once
#include <vector>
#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"
#include "Scene.h"

class SceneViewerClient
{
public:
	SceneViewerClient(std::shared_ptr< grpc::Channel> channel);
	void LoadModelsInScene(int id);
	void LoadTexturesInScene(int id);
	void LoadObjectsInScene(int id);


public:
	std::vector<Scene*> scenesList;

private:
	std::unique_ptr<SceneLoader::Stub> stub;
	const int MAX_RETRIES = 5;
	const int RETRY_COOLDOWN = 3000; // 3 secs
};