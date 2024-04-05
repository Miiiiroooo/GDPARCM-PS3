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
	UnloadScene();
}


void Scene::LoadAllResourcesToOpenGL()
{

	for (auto model : modelsList)
	{
		model->LoadModelData();
		model->LoadTextureData(GL_RGBA);
	}

	isDirty = false;
}

void Scene::UnloadScene()
{
	for (size_t i = 0; i < modelsList.size(); i++)
	{
		modelsList[i]->DeleteBuffers();
	}

	for (size_t i = 0; i < modelsList.size(); i++)
	{
		delete modelsList[i];
	}

	modelsList.clear();
	modelsList.shrink_to_fit();

	loadingProgress = 0;
	isActive = false;
	isAlreadyLoaded = false;
	isDirty = true;
	isLoading = false;

}