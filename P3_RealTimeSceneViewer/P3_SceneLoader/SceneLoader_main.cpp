#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"../../common-classes/stb_image.h"

#include <iostream>
#include "SceneLoaderImpl.h"
#include "multithreading/ThreadPoolScheduler.h"

int main()
{
	// Initialize Scheduler
	ThreadPoolScheduler::GetInstance()->Initialize(2+5); 
	ThreadPoolScheduler::GetInstance()->StartScheduler(); 
	ThreadPoolScheduler::GetInstance()->start();

	// Initialize Server
	std::string server_adr = "localhost:50052"; 
	SceneLoaderImpl loader;  

	grpc::ServerBuilder builder; 

	std::cout << "SETTING UP SERVER\n"; 
	/*grpc::ResourceQuota newQuota; 
	newQuota.SetMaxThreads(5); 
	builder.SetResourceQuota(newQuota); */
	builder.AddListeningPort(server_adr, grpc::InsecureServerCredentials()); 
	builder.RegisterService(&loader); 

	std::cout << "WAITING\n";

	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());  
	server->Wait(); 

	system("pause");

	return 0;
}