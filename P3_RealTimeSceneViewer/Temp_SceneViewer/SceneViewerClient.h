#pragma once
#include <vector>
#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"
#include "Scene.h"

class SceneViewerClient
{
public:
	SceneViewerClient(std::shared_ptr< grpc::Channel> channel);
	bool LoadModelsInScene(int id);
	bool LoadTexturesInScene(int id);
	bool LoadObjectsInScene(int id);

private:
	bool CheckConnectionToServer();


public:
	std::vector<Scene*> scenesList;

private:
	std::unique_ptr<SceneLoader::Stub> stub;
	std::shared_ptr<grpc::Channel> channel; 
	
	const int CONNECT_TO_SERVER_DURATION = 1500; // in ms
	const int MAX_RETRIES = 5;
	const int RETRY_COOLDOWN = 5000; // in ms
	const int DEFAULT_STREAM_MODELS_DEADLINE = 45; // in seconds
	const int DEFAULT_STREAM_TEXTURES_DEADLINE = 75;
	const int DEFAULT_STREAM_OBJECTS_DEADLINE = 20;
};