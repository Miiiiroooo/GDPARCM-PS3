#pragma once
#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"
#include "opengl/Model3D.h"

#include <unordered_map>
#include "SceneLoadProgress.h"

class SceneLoaderImpl final : public SceneLoader::Service  
{
public:
	SceneLoaderImpl();
	grpc::Status LoadModelsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ModelData>* writer) override;
	grpc::Status LoadTexturesInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<TextureData>* writer) override;
	grpc::Status LoadObjectsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ObjectData>* writer) override;
	grpc::Status GetSceneProgress(grpc::ServerContext* context, const IntValue* request, FloatValue* response) override;

private:
	void ResetSceneProgress(int id);


private:
	std::unordered_map<int, SceneLoadProgress> scenesProgressMap;
};