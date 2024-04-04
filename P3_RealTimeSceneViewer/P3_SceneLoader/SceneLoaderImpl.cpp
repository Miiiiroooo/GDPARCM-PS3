#include "SceneLoaderImpl.h"
#include <rapidjson/document.h>
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
	ServerSemaphore::jsonFileSem.acquire();
	FILE* inFile; 
	fopen_s(&inFile, "SceneLoaderData.json", "rb"); 
	assert(inFile != NULL); 

	char readBuffer[4096]; 
	rapidjson::FileReadStream jsonFile(inFile, readBuffer, sizeof(readBuffer)); 
	rapidjson::Document doc; 

	doc.ParseStream(jsonFile); 
	fclose(inFile); 
	ServerSemaphore::jsonFileSem.release();

	// Assign max progress with scene ID
	std::string sceneID = "Scene" + std::to_string(request->value());
	rapidjson::Value& model_itr = doc[sceneID.c_str()]["Models"];

	ResetSceneProgress(request->value()); // ensure all previous data is wiped
	ServerSemaphore::sceneProgressSem.acquire(); 
	scenesProgressMap[request->value()].modelsMaxProgress = model_itr.GetObj().MemberCount();  
	ServerSemaphore::sceneProgressSem.release(); 

	std::cout << "LOADING MODELS ON" << sceneID << "\n";
	
	// Loop through each models from JSON data
	for (rapidjson::Value::ConstMemberIterator data_itr = doc[sceneID.c_str()]["Models"].MemberBegin(); data_itr != doc[sceneID.c_str()]["Models"].MemberEnd(); ++data_itr)
	{
		// Check if server did not meet the deadline OR client suddenly disconnected
		if (context->IsCancelled())
		{
			ResetSceneProgress(request->value());
			return grpc::Status::CANCELLED;
		}

		// Prepare to stream the data of the model
		std::string modelName = data_itr->name.GetString();
		std::string modelPath = data_itr->value.GetString();
		
		ModelReference* ref = new ModelReference(modelName, modelPath);
		ref->LoadModel(); 
		std::vector<float> data = ref->GetFullVertexData(); 

		for (int i = 0; i < data.size(); i += 8) 
		{
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

			writer->Write(mData);  
		}

		// Update progress
		ServerSemaphore::sceneProgressSem.acquire(); 
		scenesProgressMap[request->value()].progress++; 
		ServerSemaphore::sceneProgressSem.release(); 
	}

	std::cout << "STATUS OK \n";

	return grpc::Status::OK;
}

grpc::Status SceneLoaderImpl::LoadTexturesInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<TextureData>* writer)
{
	// Parse JSON data for loading
	ServerSemaphore::jsonFileSem.acquire();
	FILE* inFile; 
	fopen_s(&inFile, "SceneLoaderData.json", "rb"); 
	assert(inFile != NULL);

	char readBuffer[4096];
	rapidjson::FileReadStream jsonFile(inFile, readBuffer, sizeof(readBuffer));
	rapidjson::Document doc;

	doc.ParseStream(jsonFile);
	fclose(inFile);
	ServerSemaphore::jsonFileSem.release();

	// Assign max progress with scene ID
	std::string sceneID = "Scene" + std::to_string(request->value()); 
	rapidjson::Value& texture_itr = doc[sceneID.c_str()]["Textures"];

	ServerSemaphore::sceneProgressSem.acquire();
	int maxTextures = texture_itr.GetObj().MemberCount(); 
	scenesProgressMap[request->value()].texturesMaxProgress = maxTextures; 
	ServerSemaphore::sceneProgressSem.release();

	std::cout << "LOADING TEXTURES ON " << sceneID << "\n"; 

	// Loop through each textures from JSON data
	for (rapidjson::Value::ConstMemberIterator data_itr = doc[sceneID.c_str()]["Textures"].MemberBegin(); data_itr != doc[sceneID.c_str()]["Textures"].MemberEnd(); ++data_itr) 
	{
		// Check if server did not meet the deadline OR client suddenly disconnected
		if (context->IsCancelled()) 
		{
			ResetSceneProgress(request->value()); 
			return grpc::Status::CANCELLED; 
		}

		/*std::string textureName = data_itr->name.GetString(); 
		std::string texturePath = data_itr->value.GetString(); 

		StreamTextureTask* task = new StreamTextureTask(request->value(), maxTextures, textureName, texturePath, writer);
		ThreadPoolScheduler::GetInstance()->ScheduleTask(task);*/

		// Prepare to stream the data of the texture
		std::string textureName = data_itr->name.GetString(); 
		std::string texturePath = data_itr->value.GetString(); 

		Texture* texture = new Texture(textureName, texturePath.c_str()); 
		texture->LoadTexture(GL_RGBA); 
		unsigned char* tex_bytes = texture->GetTextureBytes(); 

		int width = texture->GetWidth(); 
		int height = texture->GetHeight(); 

		int index = 0; 
		for (int i = 0; i < width; i++) 
		{
			for (int j = 0; j < height; j++) 
			{
				unsigned bytePerPixel = 4; 
				unsigned char* pixelOffset = tex_bytes + (j + width * i) * bytePerPixel; 
				unsigned int r = (unsigned int)(pixelOffset[0] & 0xff); 
				unsigned int g = (unsigned int)(pixelOffset[1] & 0xff); 
				unsigned int b = (unsigned int)(pixelOffset[2] & 0xff); 
				unsigned int a = (unsigned int)(pixelOffset[3] & 0xff); 

				PixelData* pixelData = new PixelData(); 
				pixelData->set_r(r); 
				pixelData->set_g(g);  
				pixelData->set_b(b); 
				pixelData->set_a(a);  

				TextureData textureData = TextureData();  
				textureData.set_texturename(textureName); 
				textureData.set_width(width);  
				textureData.set_height(height);  
				textureData.set_hasalpha(true);  
				textureData.set_pixelindex(index);  
				textureData.set_allocated_pixeldata(pixelData);  
				writer->Write(textureData);  

				index++;  
			}
		}

		// Update progress
		ServerSemaphore::sceneProgressSem.acquire();
		scenesProgressMap[request->value()].progress++;
		ServerSemaphore::sceneProgressSem.release();
	}

	/*switch (request->value())
	{
	case 1:
		ServerSemaphore::finishedStreamTextureSem1.acquire();
		break;
	case 2:
		ServerSemaphore::finishedStreamTextureSem2.acquire();
		break;
	case 3:
		ServerSemaphore::finishedStreamTextureSem3.acquire();
		break;
	case 4:
		ServerSemaphore::finishedStreamTextureSem4.acquire();
		break;
	case 5:
		ServerSemaphore::finishedStreamTextureSem5.acquire();
		break;
	default:
		break;
	}*/

	std::cout << "STATUS OK \n";

	return grpc::Status::OK;
}

grpc::Status SceneLoaderImpl::LoadObjectsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ObjectData>* writer)
{
	// Parse JSON data for loading
	ServerSemaphore::jsonFileSem.acquire(); 
	FILE* inFile; 
	fopen_s(&inFile, "SceneLoaderData.json", "rb"); 
	assert(inFile != NULL); 

	char readBuffer[4096]; 
	rapidjson::FileReadStream jsonFile(inFile, readBuffer, sizeof(readBuffer)); 
	rapidjson::Document doc; 

	doc.ParseStream(jsonFile); 
	fclose(inFile); 
	ServerSemaphore::jsonFileSem.release(); 

	// Get all the names of the models in the scene
	std::vector<std::string> modelNamesList;
	std::string sceneID = "Scene" + std::to_string(request->value());
	for (rapidjson::Value::ConstMemberIterator data_itr = doc[sceneID.c_str()]["Models"].MemberBegin(); data_itr != doc[sceneID.c_str()]["Models"].MemberEnd(); ++data_itr)
	{
		modelNamesList.push_back(data_itr->name.GetString());
	}

	// Initialize random values
	rapidjson::Value& objDataPtr = doc[sceneID.c_str()]["Objects"];

	int minNum = objDataPtr["Number"][0].GetInt();
	int maxNum = objDataPtr["Number"][1].GetInt();
	int randNum = Utils::GetRandomInt(minNum, maxNum);

	ServerSemaphore::sceneProgressSem.acquire(); 
	scenesProgressMap[request->value()].objectsMaxProgress = randNum;
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

	std::cout << "LOADING OBJECTS ON" << sceneID << "\n";

	// Initialize each object based on random values
	for (int i = 0; i < randNum; i++)
	{
		// Check if server did not meet the deadline OR client suddenly disconnected
		if (context->IsCancelled()) 
		{
			ResetSceneProgress(request->value()); 
			return grpc::Status::CANCELLED; 
		}

		// Prepare to stream the data of the object
		int randModelIndex = Utils::GetRandomInt(0, modelNamesList.size() - 1);
		std::string modelName = modelNamesList[randModelIndex];

		float scaleX = objDataPtr["Scalings"][modelName.c_str()][0].GetFloat();
		float scaleY = objDataPtr["Scalings"][modelName.c_str()][1].GetFloat();
		float scaleZ = objDataPtr["Scalings"][modelName.c_str()][2].GetFloat();

		Vector3* pos = new Vector3(); 
		pos->set_x(Utils::GetRandomFloat(minPosX, maxPosX)); 
		pos->set_y(Utils::GetRandomFloat(minPosY, maxPosY)); 
		pos->set_z(Utils::GetRandomFloat(minPosZ, maxPosZ)); 

		Vector3* rot = new Vector3(); 
		rot->set_x(Utils::GetRandomFloat(minRotX, maxRotX));
		rot->set_y(Utils::GetRandomFloat(minRotY, maxRotY)); 
		rot->set_z(Utils::GetRandomFloat(minRotZ, maxRotZ)); 

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

		writer->Write(oData); 

		// Update progress
		ServerSemaphore::sceneProgressSem.acquire();
		scenesProgressMap[request->value()].progress++; 
		ServerSemaphore::sceneProgressSem.release();
	}

	std::cout << "STATUS OK \n";

	return grpc::Status::OK; 
}

grpc::Status SceneLoaderImpl::GetSceneProgress(grpc::ServerContext* context, const IntValue* request, FloatValue* response)
{
	int id = request->value();
	float progressInPercent = 0;

	ServerSemaphore::sceneProgressSem.acquire(); 
	SceneLoadProgress progressData = scenesProgressMap[id];
	int currentProgress = progressData.progress;
	int maxModels = progressData.modelsMaxProgress;
	int maxTextures = progressData.texturesMaxProgress;
	int maxObjects = progressData.objectsMaxProgress;
	ServerSemaphore::sceneProgressSem.release();

	if (maxModels == -1)
	{
		response->set_value(progressInPercent); 
		return grpc::Status::OK;
	}

	float total = maxModels;
	progressInPercent += ((float)currentProgress / total) * 33.f; 

	if (maxTextures == -1)
	{
		response->set_value(progressInPercent); 
		return grpc::Status::OK; 
	}

	total = maxModels + maxTextures;
	progressInPercent += ((float)currentProgress / total) * 33.f;

	if (maxObjects == -1) 
	{
		response->set_value(progressInPercent); 
		return grpc::Status::OK; 
	}

	total = maxModels + maxTextures + maxObjects; 
	progressInPercent += ((float)currentProgress / total) * 34.f;

	response->set_value(progressInPercent);  
	return grpc::Status::OK; 
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