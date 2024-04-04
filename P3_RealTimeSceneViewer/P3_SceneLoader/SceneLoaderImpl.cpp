#include "SceneLoaderImpl.h"
#include "Utils.h"
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

SceneLoaderImpl::SceneLoaderImpl()
{

}

grpc::Status SceneLoaderImpl::LoadModelsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ModelData>* writer)
{
	// APPLY MUTEX
	FILE* inFile; 
	fopen_s(&inFile, "SceneLoaderData.json", "rb"); 
	assert(inFile != NULL); 

	char readBuffer[4096]; 
	rapidjson::FileReadStream jsonFile(inFile, readBuffer, sizeof(readBuffer)); 
	rapidjson::Document doc; 

	doc.ParseStream(jsonFile); 
	fclose(inFile); 

	std::cout << "LOADING VERTICES\n";

	std::string sceneID = "Scene" + std::to_string(request->value());
	rapidjson::Value& model_itr = doc[sceneID.c_str()]["Models"];
	scenesProgressMap[request->value()].modelsMaxProgress = model_itr.GetObj().MemberCount(); 

	for (rapidjson::Value::ConstMemberIterator data_itr = doc[sceneID.c_str()]["Models"].MemberBegin(); data_itr != doc[sceneID.c_str()]["Models"].MemberEnd(); ++data_itr)
	{
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

		scenesProgressMap[request->value()].progress++; 
	}

	std::cout << "STATUS OK \n";

	return grpc::Status::OK;
}

grpc::Status SceneLoaderImpl::LoadTexturesInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<TextureData>* writer)
{
	// APPLY MUTEX
	FILE* inFile; 
	fopen_s(&inFile, "SceneLoaderData.json", "rb"); 
	assert(inFile != NULL);

	char readBuffer[4096];
	rapidjson::FileReadStream jsonFile(inFile, readBuffer, sizeof(readBuffer));
	rapidjson::Document doc;

	doc.ParseStream(jsonFile);
	fclose(inFile);

	std::cout << "LOADING TEXTURE\n";

	std::string sceneID = "Scene" + std::to_string(request->value()); 
	rapidjson::Value& texture_itr = doc[sceneID.c_str()]["Textures"]; 
	scenesProgressMap[request->value()].texturesMaxProgress = texture_itr.GetObj().MemberCount();  

	for (rapidjson::Value::ConstMemberIterator data_itr = doc[sceneID.c_str()]["Textures"].MemberBegin(); data_itr != doc[sceneID.c_str()]["Textures"].MemberEnd(); ++data_itr) 
	{
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

		scenesProgressMap[request->value()].progress++;
	}

	std::cout << "STATUS OK \n";

	return grpc::Status::OK;
}

grpc::Status SceneLoaderImpl::LoadObjectsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ObjectData>* writer)
{
	// APPLY MUTEX
	FILE* inFile;  
	fopen_s(&inFile, "SceneLoaderData.json", "rb"); 
	assert(inFile != NULL); 

	char readBuffer[4096]; 
	rapidjson::FileReadStream jsonFile(inFile, readBuffer, sizeof(readBuffer)); 
	rapidjson::Document doc; 

	doc.ParseStream(jsonFile); 
	fclose(inFile); 

	std::cout << "LOADING OBJECT\n";

	// get names of the models in the scene
	std::vector<std::string> modelNamesList;
	std::string sceneID = "Scene" + std::to_string(request->value());
	for (rapidjson::Value::ConstMemberIterator data_itr = doc[sceneID.c_str()]["Models"].MemberBegin(); data_itr != doc[sceneID.c_str()]["Models"].MemberEnd(); ++data_itr)
	{
		modelNamesList.push_back(data_itr->name.GetString());
	}

	// initialize random values
	rapidjson::Value& objDataPtr = doc[sceneID.c_str()]["Objects"];

	int minNum = objDataPtr["Number"][0].GetInt();
	int maxNum = objDataPtr["Number"][1].GetInt();
	int randNum = Utils::GetRandomInt(minNum, maxNum);
	scenesProgressMap[request->value()].objectsMaxProgress = randNum; 

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


	for (int i = 0; i < randNum; i++)
	{
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

		scenesProgressMap[request->value()].progress++; 
	}

	std::cout << "STATUS OK \n";

	return grpc::Status::OK; 
}

grpc::Status SceneLoaderImpl::GetSceneProgress(grpc::ServerContext* context, const IntValue* request, FloatValue* response)
{
	int id = request->value();
	float progressInPercent = 0;

	SceneLoadProgress progressData = scenesProgressMap[id]; 

	if (progressData.modelsMaxProgress == -1) 
	{
		response->set_value(progressInPercent); 
		return grpc::Status::OK;
	}

	float total = progressData.modelsMaxProgress; 
	progressInPercent += ((float)progressData.progress / total) * 33.f; 

	if (progressData.texturesMaxProgress == -1)
	{
		response->set_value(progressInPercent); 
		return grpc::Status::OK; 
	}

	total = progressData.modelsMaxProgress + progressData.texturesMaxProgress;
	progressInPercent += ((float)progressData.progress / total) * 33.f; 

	if (progressData.objectsMaxProgress == -1) 
	{
		response->set_value(progressInPercent); 
		return grpc::Status::OK; 
	}

	total  = progressData.modelsMaxProgress + progressData.texturesMaxProgress + progressData.objectsMaxProgress;
	progressInPercent += ((float)progressData.progress / total) * 34.f;

	response->set_value(progressInPercent);  
	return grpc::Status::OK; 
}
