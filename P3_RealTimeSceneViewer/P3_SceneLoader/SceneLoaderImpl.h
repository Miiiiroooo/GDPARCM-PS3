#pragma once
#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"
#include "opengl/Model3D.h"

#include <rapidjson/document.h>
#include <unordered_map>
#include <random>
#include "SceneLoadProgress.h"

class SceneLoaderImpl final : public SceneLoader::Service  
{
public:
	SceneLoaderImpl();
	grpc::Status LoadModelsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ModelData>* writer) override;
	grpc::Status LoadTexturesInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<TextureData>* writer) override;
	grpc::Status LoadObjectsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ObjectData>* writer) override;

private:
	void StreamModels(int sceneID, std::string modelName, std::string modelPath, grpc::ServerWriter<ModelData>* writer);
	void StreamTextures(int sceneID, std::string textureName, std::string texturePath, grpc::ServerWriter<TextureData>* writer);

	rapidjson::Document ParseJSONData();
	void InitializeRandomizers(int sceneID);
	float GetSceneProgress(int id);
	void ResetSceneProgress(int id);


private:
	const int BATCH_SIZE_MAX = 8;
	std::unordered_map<int, SceneLoadProgress> scenesProgressMap;
	std::unordered_map<int, std::random_device*> randDevicesMap;
	std::unordered_map<int, std::mt19937*> randGeneratorsMap;
};