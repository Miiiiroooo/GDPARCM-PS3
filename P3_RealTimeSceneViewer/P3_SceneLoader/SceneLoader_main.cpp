#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"../../common-classes/stb_image.h"

#include <chrono>
#include <thread>
#include <cmath>
#include <iostream>
#include "../common-classes/opengl/ModelReference.h"


int main()
{
	srand(time(0));

	if (!glfwInit()) 
		return -1;

	auto start_time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 10; i++)
	{
		ModelReference* ref = new ModelReference("../3D/amumu.obj"); 
		ref->LoadModel(false); 
	}


	ModelReference* ref = new ModelReference("../3D/amumu.obj"); 
	ref->LoadModel(false); 
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
	std::cout << "\n\nmilliseconds:" << time / std::chrono::milliseconds(1) << "\n";
	system("pause");

	return 0;
}