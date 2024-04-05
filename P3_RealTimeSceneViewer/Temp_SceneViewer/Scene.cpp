#include "Scene.h"

Scene::Scene(int id) : id(id)
{
	isAlreadyLoaded = false;
	isDirty = true;
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
	}

	for (auto texture : texturesList)
	{
		texture->LoadTextureData(GL_RGBA); 
	}

	isDirty = false;
}

void Scene::UnloadScene()
{
	for (size_t i = 0; i < modelsList.size(); i++) 
	{
		delete modelsList[i]; 
	}

	modelsList.clear(); 
	modelsList.shrink_to_fit(); 

	for (size_t i = 0; i < texturesList.size(); i++) 
	{
		delete texturesList[i]; 
	}

	texturesList.clear(); 
	texturesList.shrink_to_fit(); 

	for (size_t i = 0; i < objectsList.size(); i++) 
	{
		delete objectsList[i]; 
	}

	objectsList.clear(); 
	objectsList.shrink_to_fit(); 

	isAlreadyLoaded = false;
	isDirty = true;
}