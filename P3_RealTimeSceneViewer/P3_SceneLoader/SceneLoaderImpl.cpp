#include "SceneLoaderImpl.h"
#include <rapidjson/filereadstream.h>
#include "multithreading/ThreadPoolScheduler.h"
#include "StreamTextureTask.h"
#include "ServerSemaphore.h"
#include "Utils.h"


SceneLoaderImpl::SceneLoaderImpl()
{

}

grpc::Status SceneLoaderImpl::LoadModelsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ModelData>* writer)
{
	// Parse JSON data for loading
	rapidjson::Document doc = ParseJSONData(); 

	// Assign max progress with scene ID
	std::string sceneID_str = "Scene" + std::to_string(request->value());
	rapidjson::Value& model_itr = doc[sceneID_str.c_str()]["Models"];

	ResetSceneProgress(request->value()); // ensure all previous data is wiped
	ServerSemaphore::sceneProgressSem.acquire(); 
	scenesProgressMap[request->value()].modelsMaxProgress = model_itr.GetObj().MemberCount();  
	ServerSemaphore::sceneProgressSem.release(); 

	std::cout << "STREAMING MODELS ON " << sceneID_str << "\n";
	

	// Loop through each models from JSON data
	for (rapidjson::Value::ConstMemberIterator data_itr = doc[sceneID_str.c_str()]["Models"].MemberBegin(); data_itr != doc[sceneID_str.c_str()]["Models"].MemberEnd(); ++data_itr)
	{
		// Check if server did not meet the deadline OR client suddenly disconnected
		if (context->IsCancelled())
		{
			std::cout << "RPC Call has been CANCELLED on " << sceneID_str << "\n";
			ResetSceneProgress(request->value());
			return grpc::Status::CANCELLED;
		}

		// Prepare to stream the data of the model
		std::string modelName = data_itr->name.GetString();
		std::string modelPath = data_itr->value.GetString();
		
		StreamModels(request->value(), modelName, modelPath, writer);

		// Update progress
		ServerSemaphore::sceneProgressSem.acquire(); 
		scenesProgressMap[request->value()].progress++; 
		ServerSemaphore::sceneProgressSem.release(); 
	}

	std::cout << "STATUS OK WHEN STREAMING MODELS ON " << sceneID_str << "\n";

	return grpc::Status::OK;
}

grpc::Status SceneLoaderImpl::LoadTexturesInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<TextureData>* writer)
{
	// Parse JSON data for loading
	rapidjson::Document doc = ParseJSONData();

	// Assign max progress with scene ID
	std::string sceneID_str = "Scene" + std::to_string(request->value()); 
	rapidjson::Value& texture_itr = doc[sceneID_str.c_str()]["Textures"];

	ServerSemaphore::sceneProgressSem.acquire();
	int maxTextures = texture_itr.GetObj().MemberCount(); 
	scenesProgressMap[request->value()].texturesMaxProgress = maxTextures; 
	ServerSemaphore::sceneProgressSem.release();

	std::cout << "STREAMING TEXTURES ON " << sceneID_str << "\n"; 


	// Loop through each textures from JSON data
	for (rapidjson::Value::ConstMemberIterator data_itr = doc[sceneID_str.c_str()]["Textures"].MemberBegin(); data_itr != doc[sceneID_str.c_str()]["Textures"].MemberEnd(); ++data_itr) 
	{
		// Check if server did not meet the deadline OR client suddenly disconnected
		if (context->IsCancelled()) 
		{
			std::cout << "RPC Call has been CANCELLED on " << sceneID_str << "\n";
			ResetSceneProgress(request->value()); 
			return grpc::Status::CANCELLED; 
		}

		// Prepare to stream the data of the texture
		std::string textureName = data_itr->name.GetString(); 
		std::string texturePath = data_itr->value.GetString(); 

		StreamTextures(request->value(), textureName, texturePath, writer);

		// Update progress
		ServerSemaphore::sceneProgressSem.acquire();
		scenesProgressMap[request->value()].progress++;
		ServerSemaphore::sceneProgressSem.release();
	}

	std::cout << "STATUS OK WHEN STREAMING TEXTURES ON " << sceneID_str << "\n";

	return grpc::Status::OK;
}

grpc::Status SceneLoaderImpl::LoadObjectsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ObjectData>* writer)
{
	// Parse JSON data for loading
	rapidjson::Document doc = ParseJSONData();

	// Get all the names of the models in the scene
	std::vector<std::string> modelNamesList;
	int sceneID = request->value();
	std::string sceneID_str = "Scene" + std::to_string(sceneID);
	for (rapidjson::Value::ConstMemberIterator data_itr = doc[sceneID_str.c_str()]["Models"].MemberBegin(); data_itr != doc[sceneID_str.c_str()]["Models"].MemberEnd(); ++data_itr)
	{
		modelNamesList.push_back(data_itr->name.GetString());
	}

	// Initialize random values
	rapidjson::Value& objDataPtr = doc[sceneID_str.c_str()]["Objects"];
	InitializeRandomizers(sceneID);
	std::mt19937* gen = randGeneratorsMap[sceneID]; 

	int minNum = objDataPtr["Number"][0].GetInt();
	int maxNum = objDataPtr["Number"][1].GetInt();
	int randNum = Utils::GetRandomInt(minNum, maxNum, gen);

	ServerSemaphore::sceneProgressSem.acquire();
	scenesProgressMap[sceneID].objectsMaxProgress = randNum;
	ServerSemaphore::sceneProgressSem.release();

	float minPosX = objDataPtr["Positions"]["randX"][0].GetFloat();
	float maxPosX = objDataPtr["Positions"]["randX"][1].GetFloat();
	float minPosY = objDataPtr["Positions"]["randY"][0].GetFloat();
	float maxPosY = objDataPtr["Positions"]["randY"][1].GetFloat();
	float minPosZ = objDataPtr["Positions"]["randZ"][0].GetFloat();
	float maxPosZ = objDataPtr["Positions"]["randZ"][1].GetFloat();

	float minRotX = objDataPtr["Rotations"]["randX"][0].GetFloat();
	float maxRotX = objDataPtr["Rotations"]["randX"][1].GetFloat();
	float minRotY = objDataPtr["Rotations"]["randY"][0].GetFloat();
	float maxRotY = objDataPtr["Rotations"]["randY"][1].GetFloat();
	float minRotZ = objDataPtr["Rotations"]["randZ"][0].GetFloat();
	float maxRotZ = objDataPtr["Rotations"]["randZ"][1].GetFloat();

	std::cout << "STREAMING OBJECTS ON " << sceneID_str << "\n";


	// Initialize each object based on random values
	for (int i = 0; i < randNum; i++)
	{
		// Check if server did not meet the deadline OR client suddenly disconnected
		if (context->IsCancelled())
		{
			std::cout << "RPC Call has been CANCELLED on " << sceneID_str << "\n";
			ResetSceneProgress(sceneID);
			return grpc::Status::CANCELLED;
		}

		// Update progress
		ServerSemaphore::sceneProgressSem.acquire(); 
		scenesProgressMap[sceneID].progress++;  
		ServerSemaphore::sceneProgressSem.release(); 

		float progress = GetSceneProgress(sceneID); // progress of already loaded objects
		//std::cout << progress << "\n";


		// Prepare to stream the data of the object
		int randModelIndex = Utils::GetRandomInt(0, modelNamesList.size() - 1, gen); 
		std::string modelName = modelNamesList[randModelIndex];

		float scaleX = objDataPtr["Scalings"][modelName.c_str()][0].GetFloat();
		float scaleY = objDataPtr["Scalings"][modelName.c_str()][1].GetFloat();
		float scaleZ = objDataPtr["Scalings"][modelName.c_str()][2].GetFloat();

		Vector3* pos = new Vector3();
		pos->set_x(Utils::GetRandomFloat(minPosX, maxPosX, gen));
		pos->set_y(Utils::GetRandomFloat(minPosY, maxPosY, gen));
		pos->set_z(Utils::GetRandomFloat(minPosZ, maxPosZ, gen));

		Vector3* rot = new Vector3();
		rot->set_x(Utils::GetRandomFloat(minRotX, maxRotX, gen));
		rot->set_y(Utils::GetRandomFloat(minRotY, maxRotY, gen));
		rot->set_z(Utils::GetRandomFloat(minRotZ, maxRotZ, gen));

		Vector3* scale = new Vector3();
		scale->set_x(scaleX);
		scale->set_y(scaleY);
		scale->set_z(scaleZ);

		ObjectData oData;
		oData.set_modelname(modelName);
		oData.set_texturename(modelName);
		oData.set_allocated_position(pos);
		oData.set_allocated_rotation(rot);
		oData.set_allocated_scale(scale);
		oData.set_sceneprogress(progress);

		writer->Write(oData);
	}

	std::cout << "STATUS OK WHEN STREAMING OBJECTS ON " << sceneID_str << "\n";

	return grpc::Status::OK; 
}

void SceneLoaderImpl::StreamModels(int sceneID, std::string modelName, std::string modelPath, grpc::ServerWriter<ModelData>* writer)
{
	ModelReference* ref = new ModelReference(modelName, modelPath);
	ref->LoadModel();
	std::vector<float> data = ref->GetFullVertexData();

	for (int i = 0; i < data.size(); i += 8)
	{
		float progress = GetSceneProgress(sceneID); // progress of already loaded models
		ServerSemaphore::sceneProgressSem.acquire();
		int maxModels = scenesProgressMap[sceneID].modelsMaxProgress; 
		ServerSemaphore::sceneProgressSem.release();
		progress += (float)i / (float)data.size() * (1.f / (float)maxModels) * 33.f;
		//std::cout << progress << "\n";

		Vector3* pos = new Vector3();
		pos->set_x(data[i]);
		pos->set_y(data[i + 1]);
		pos->set_z(data[i + 2]);

		Vector3* normals = new Vector3();
		normals->set_x(data[i + 3]);
		normals->set_y(data[i + 4]);
		normals->set_z(data[i + 5]);

		VertexData* vData = new VertexData();
		vData->set_allocated_position(pos);
		vData->set_allocated_normals(normals);
		vData->set_u(data[i + 6]);
		vData->set_v(data[i + 7]);

		ModelData mData;
		mData.set_modelname(modelName);
		mData.set_vdataindex(i / 8);
		mData.set_allocated_vdata(vData);
		mData.set_sceneprogress(progress);

		writer->Write(mData);
	}
}

void SceneLoaderImpl::StreamTextures(int sceneID, std::string textureName, std::string texturePath, grpc::ServerWriter<TextureData>* writer)
{
	Texture* texture = new Texture(textureName, texturePath.c_str());
	texture->LoadTexture(GL_RGBA);
	unsigned char* tex_bytes = texture->GetTextureBytes();

	int width = texture->GetWidth();
	int height = texture->GetHeight();
	unsigned bytesPerPixel = 4;

	int index = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j += BATCH_SIZE_MAX)
		{
			float progress = GetSceneProgress(sceneID); // progress of already loaded textures
			ServerSemaphore::sceneProgressSem.acquire(); 
			int maxTextures = scenesProgressMap[sceneID].texturesMaxProgress; 
			ServerSemaphore::sceneProgressSem.release(); 
			progress += (float)(i * height + j) / (float)(width * height) * (1.f / maxTextures) * 33.f;
			//std::cout << progress << "\n";

			unsigned char* pixelOffset = tex_bytes + (j + width * i) * bytesPerPixel;

			TextureData textureData = TextureData();
			textureData.set_texturename(textureName);
			textureData.set_width(width);
			textureData.set_height(height);
			textureData.set_hasalpha(true);
			textureData.set_pixelindex(index);
			textureData.set_sceneprogress(progress);

			for (int k = 0; k < BATCH_SIZE_MAX * bytesPerPixel; k += bytesPerPixel)
			{
				if (j + textureData.pixeldatabatch_size() == height)
				{
					break;
				}

				unsigned int r = (unsigned int)(pixelOffset[k] & 0xff);
				unsigned int g = (unsigned int)(pixelOffset[k + 1] & 0xff);
				unsigned int b = (unsigned int)(pixelOffset[k + 2] & 0xff);
				unsigned int a = (unsigned int)(pixelOffset[k + 3] & 0xff);

				PixelData* pixelData = textureData.add_pixeldatabatch();
				pixelData->set_r(r);
				pixelData->set_g(g);
				pixelData->set_b(b);
				pixelData->set_a(a);
			}

			writer->Write(textureData);

			index += BATCH_SIZE_MAX;
		}
	}
}

rapidjson::Document SceneLoaderImpl::ParseJSONData()
{
	rapidjson::Document doc;

	ServerSemaphore::jsonFileSem.acquire();
	FILE* inFile;
	fopen_s(&inFile, "SceneLoaderData.json", "rb");
	assert(inFile != NULL);

	char readBuffer[4096];
	rapidjson::FileReadStream jsonFile(inFile, readBuffer, sizeof(readBuffer));

	doc.ParseStream(jsonFile);
	fclose(inFile);
	ServerSemaphore::jsonFileSem.release();

	return doc;
}

void SceneLoaderImpl::InitializeRandomizers(int sceneID)
{
	if (randDevicesMap.contains(sceneID))
	{
		return;
	}

	std::random_device* rd = new std::random_device();
	randDevicesMap[sceneID] = rd;
	randGeneratorsMap[sceneID] = new std::mt19937((*rd)());
}

float SceneLoaderImpl::GetSceneProgress(int id)
{
	ServerSemaphore::sceneProgressSem.acquire(); 
	SceneLoadProgress progressData = scenesProgressMap[id]; 
	int currentProgress = progressData.progress; 
	int maxModels = progressData.modelsMaxProgress; 
	int maxTextures = progressData.texturesMaxProgress; 
	int maxObjects = progressData.objectsMaxProgress; 
	ServerSemaphore::sceneProgressSem.release(); 


	float progressInPercent = 0;
	if (maxModels == -1) 
	{
		return progressInPercent;
	}

	float total = 0.f; 
	if (maxTextures == -1)
	{
		total = maxModels; 

		progressInPercent = ((float)currentProgress / total) * 33.f;
		return progressInPercent; 
	}

	if (maxObjects == -1)
	{
		currentProgress -= maxModels;
		total = maxTextures;

		progressInPercent = 33.f;
		progressInPercent += ((float)currentProgress / total) * 33.f;
		return progressInPercent; 
	}
	
	currentProgress -= maxModels + maxTextures;
	total = maxObjects; 

	progressInPercent = 66.f;
	progressInPercent += ((float)currentProgress / total) * 33.f;
	return progressInPercent; 
}

void SceneLoaderImpl::ResetSceneProgress(int id)
{
	ServerSemaphore::sceneProgressSem.acquire();
	scenesProgressMap[id].progress = 0;
	scenesProgressMap[id].modelsMaxProgress = -1;
	scenesProgressMap[id].texturesMaxProgress = -1;
	scenesProgressMap[id].objectsMaxProgress = -1;
	ServerSemaphore::sceneProgressSem.release(); 
}