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
	grpc::Status LoadModelsInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<ModelData>* writer) override 
	{
		std::cout << "LOADING VERTICES\n";

		ModelReference* ref = new ModelReference("../3D/amumu.obj"); 
		ref->LoadModel(); 
		std::vector<float> data = ref->GetFullVertexData(); 
		
		for (int i = 0; i < data.size(); i += 8)
		{
			Vector3* pos = new Vector3();
			pos->set_x(data[i]);
			pos->set_y(data[i+1]);
			pos->set_z(data[i+2]);

			Vector3* normals = new Vector3();
			normals->set_x(data[i+3]);
			normals->set_y(data[i+4]);
			normals->set_z(data[i+5]);

			VertexData* vData = new VertexData();
			vData->set_allocated_position(pos);
			vData->set_allocated_normals(normals);
			vData->set_u(data[i+6]);
			vData->set_v(data[i+7]); 

			ModelData mData;
			mData.set_modelname("amumu"); 
			mData.set_vdataindex(i / 8); 
			mData.set_allocated_vdata(vData); 

			writer->Write(mData); 
		}

		std::cout << "STATUS OK \n";

		return grpc::Status::OK;
	}

	grpc::Status LoadTexturesInScene(grpc::ServerContext* context, const IntValue* request, grpc::ServerWriter<TextureData>* writer) override
	{
		std::cout << "LOADING TEXTURE\n";
		
		Texture* texture = new Texture("../3D/amumu.png");
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
				textureData.set_texturename("amumu"); 
				textureData.set_width(width); 
				textureData.set_height(height); 
				textureData.set_hasalpha(true); 
				textureData.set_pixelindex(index); 
				textureData.set_allocated_pixeldata(pixelData);
				writer->Write(textureData);

				index++;
			}
		}

		std::cout << "STATUS OK \n";

		return grpc::Status::OK;
	}

	grpc::Status LoadObjectsInScene(::grpc::ServerContext* context, const ::IntValue* request, ::grpc::ServerWriter< ::ObjectData>* writer) override
	{

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