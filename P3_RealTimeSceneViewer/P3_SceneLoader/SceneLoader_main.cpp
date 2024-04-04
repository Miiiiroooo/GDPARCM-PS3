#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"../../common-classes/stb_image.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include "SceneLoaderImpl.h"

int main()
{
	srand(time(0));

	std::string server_adr = "localhost:50052"; 
	SceneLoaderImpl loader;  

	grpc::ServerBuilder builder; 

	std::cout << "SETTING UP SERVER\n"; 
	grpc::ResourceQuota newQuota; 
	newQuota.SetMaxThreads(5); 
	builder.SetResourceQuota(newQuota); 
	builder.AddListeningPort(server_adr, grpc::InsecureServerCredentials()); 
	builder.RegisterService(&loader); 

	std::cout << "WAITING\n";

	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());  
	server->Wait(); 

	system("pause");

	return 0;
}