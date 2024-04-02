#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"../../common-classes/stb_image.h"

#include <chrono>
#include <thread>
#include <cmath>
#include <iostream>
#include "../common-classes/opengl/ModelReference.h"

#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"


class SceneLoaderImpl final : public SceneLoader::Service
{
public:
	grpc::Status LoadObjectsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ObjectData>* writer) override
	{
		ModelReference* ref = new ModelReference("../3D/amumu.obj"); 
		ref->LoadModel(); 
		std::vector<float> data = ref->GetFullVertexData(); 

		for (int i = 0; i < data.size(); i += 8)
		{
			VertexData vData;
			vData.set_vx(data[i]);
			vData.set_vy(data[i+1]);
			vData.set_vz(data[i+2]);
			vData.set_nx(data[i+3]);
			vData.set_nx(data[i+4]);
			vData.set_nx(data[i+5]);
			vData.set_u(data[i+6]);
			vData.set_v(data[i+7]);

			ObjectData oData;
			oData.set_objname("amumu");  
			oData.set_vdataindex(i / 8); 
			oData.set_allocated_vdata(&vData);

			writer->Write(oData);
		}

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

	std::string server_adr = "0.0.0.0:50051";
	SceneLoaderImpl loader;

	grpc::ServerBuilder builder;

	builder.AddListeningPort(server_adr, grpc::InsecureServerCredentials());
	builder.RegisterService(&loader);

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