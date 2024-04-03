#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"../../common-classes/stb_image.h"

#include <chrono>
#include <thread>
#include <cmath>
#include <iostream>
#include "../common-classes/opengl/ModelReference.h"
#include "../common-classes/opengl/Texture.h"

#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"


class SceneLoaderImpl final : public SceneLoader::Service
{
public:
	grpc::Status LoadObjectsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ObjectData>* writer) override
	{
		std::cout << "BOUT TO LOAD\n";

		ModelReference* ref = new ModelReference("../3D/amumu.obj"); 
		ref->LoadModel(); 
		std::vector<float> data = ref->GetFullVertexData(); 

		for (int i = 0; i < data.size(); i += 8)
		{
			VertexData* vData = new VertexData();
			vData->set_vx(data[i]); 
			vData->set_vy(data[i+1]); 
			vData->set_vz(data[i+2]); 
			vData->set_nx(data[i+3]); 
			vData->set_nx(data[i+4]); 
			vData->set_nx(data[i+5]); 
			vData->set_u(data[i+6]); 
			vData->set_v(data[i+7]); 

			ObjectData oData;
			oData.set_objname("amumu");  
			oData.set_vdataindex(i / 8); 
			oData.set_allocated_vdata(vData);

			writer->Write(oData);
		}

		std::cout << "STATUS OK \n";

		return grpc::Status::OK;
	}

	grpc::Status LoadTexturesInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<TextureData>* writer) override
	{
		std::cout << "BOUT TO LOAD\n";

		Texture* texture = new Texture("../3D/amumu.png");
		texture->LoadTexture(GL_RGBA);
		const char* tex_bytes_to_char = reinterpret_cast<char const*>(texture->GetTextureBytes()); 
		std::string* tex_bytes_to_str = new std::string(tex_bytes_to_char, strlen(tex_bytes_to_char));  

		TextureData data;
		data.set_texturename("amumu");
		data.set_width(texture->GetWidth());
		data.set_height(texture->GetHeight());
		data.set_hasalpha(true);
		data.set_allocated_texturebytes(tex_bytes_to_str); 

		writer->Write(data);

		std::cout << "STATUS OK \n";

		return grpc::Status::OK;
	}

	grpc::Status GetSceneProgress(grpc::ServerContext* context, const IntValue* request, FloatValue* response) override
	{
		return grpc::Status::OK;
	}
};


int main()
{
	srand(time(0));

	std::string server_adr = "localhost:50052";
	SceneLoaderImpl loader;

	grpc::ServerBuilder builder;

	std::cout << "SETTING UP SERVER\n";

	builder.AddListeningPort(server_adr, grpc::InsecureServerCredentials());
	builder.RegisterService(&loader);

	std::cout << "WAITING\n";

	std::unique_ptr<grpc::Server> server(builder.BuildAndStart()); 
	server->Wait();


	/*auto start_time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 10; i++)
	{
		ModelReference* ref = new ModelReference("../3D/amumu.obj"); 
		ref->LoadModel(); 
	}

	ModelReference* ref = new ModelReference("../3D/amumu.obj"); 
	ref->LoadModel(); 
	std::vector<float> data = ref->GetFullVertexData(); 

	for (int i = 0; i < data.size(); i++) 
	{
		std::cout << data[i] << " "; 
		if (i % 8 == 7) 
		{
			std::cout << "\n"; 
		}
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	std::cout << "\n\nmilliseconds:" << time / std::chrono::milliseconds(1) << "\n";*/


	system("pause");

	return 0;
}