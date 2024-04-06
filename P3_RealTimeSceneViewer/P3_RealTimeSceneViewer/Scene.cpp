#include "Scene.h"

Scene::Scene(int id) : id(id)
{
	isAlreadyLoaded = false;
	isDirty = true;
	isActive = false;
	isLoading = true;

	loadingProgress = 0.f;
}

Scene::~Scene()
{

}


void Scene::LoadAllResourcesToOpenGL()
{
	std::cout << "Models: " << modelList.size() << std::endl;
	for (auto model : modelList)
	{
		model->LoadModelData();
		model->LoadTextureData(GL_RGBA);
	}

	isDirty = false;
}

void Scene::UnloadScene()
{
	for (size_t i = 0; i < modelList.size(); i++)
	{
		modelList[i]->DeleteBuffers();
	}

	for (size_t i = 0; i < modelRef.size(); i++)
	{
		modelRef[i]->DeleteBuffers();
	}

	for (size_t i = 0; i < modelList.size(); i++)
	{
		delete modelList[i];
	}
	for (size_t i = 0; i < modelRef.size(); i++)
	{
		delete modelRef[i];
	}

	modelList.clear();
	modelList.shrink_to_fit();

	modelRef.clear();
	modelRef.shrink_to_fit();

	loadingProgress = 0;
	isActive = false;
	isAlreadyLoaded = false;
	isDirty = true;
	isLoading = false;

}