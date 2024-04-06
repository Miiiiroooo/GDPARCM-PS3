#include "Scene.h"

Scene::Scene(int id) : id(id)
{
	areResourcesStreamed = false;
	isDirty = true;
	isActive = false;
	isLoading = true;

	loadingProgress = 0.f;
}

Scene::~Scene()
{

}

float Scene::GetLoadingProgress()
{
	float progress = loadingProgress;
	return progress;
}

void Scene::SetLoadingProgress(float value)
{
	loadingProgress = value;
}


void Scene::LoadAllResourcesToOpenGL()
{
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

	for (size_t i = 0; i < unloadedModelsList.size(); i++)
	{
		unloadedModelsList[i]->DeleteBuffers();
	}

	for (size_t i = 0; i < modelList.size(); i++)
	{
		delete modelList[i];
	}
	for (size_t i = 0; i < unloadedModelsList.size(); i++)
	{
		delete unloadedModelsList[i];
	}

	modelList.clear();
	modelList.shrink_to_fit();

	unloadedModelsList.clear();
	unloadedModelsList.shrink_to_fit();

	loadingProgress = 0;
	isActive = false;
	areResourcesStreamed = false;
	isDirty = true;
	isLoading = false;

}