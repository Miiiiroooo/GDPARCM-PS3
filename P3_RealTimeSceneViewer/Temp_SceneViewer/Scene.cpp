#include "Scene.h"
#include "ClientSemaphore.h"

Scene::Scene(int id) : id(id)
{
	areResourcesStreamed = false;
	isDirty = true;
	loadingProgress = 0.f;

	switch (id)
	{
	case 1:
		progressSem = &ClientSemaphore::sceneProgressSem1;
		break;
	case 2:
		progressSem = &ClientSemaphore::sceneProgressSem2;
		break;
	case 3:
		progressSem = &ClientSemaphore::sceneProgressSem3;
		break;
	case 4:
		progressSem = &ClientSemaphore::sceneProgressSem4;
		break;
	case 5:
		progressSem = &ClientSemaphore::sceneProgressSem5;
		break;
	}
}

Scene::~Scene()
{
	UnloadScene();
}

float Scene::GetLoadingProgress()
{
	progressSem->acquire(); // 1 reader 1 writer
	float progress = loadingProgress;
	progressSem->release();
	return progress;
}

void Scene::SetLoadingProgress(float value)
{
	progressSem->acquire(); // 1 reader 1 writer
	loadingProgress = value;
	progressSem->release();
}

void Scene::LoadAllResourcesToOpenGL()
{
	int loadedResources = 0;

	for (int i = 0; i < unloadedModelsList.size(); i++)
	{
		ModelReference* model = unloadedModelsList[i];

		if (loadedResources >= MAX_SETUP_BUFFERS_IN_MAIN)
		{
			break;
		}

		if (!model->IsModelLoaded())
		{
			model->LoadModelData();
			loadedResources++;

			loadedModelsList.push_back(model);
			unloadedModelsList.erase(unloadedModelsList.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < unloadedTexturesList.size(); i++)
	{
		Texture* texture = unloadedTexturesList[i];

		if (loadedResources >= MAX_SETUP_BUFFERS_IN_MAIN)
		{
			break;
		}

		if (!texture->IsTextureLoaded())
		{
			texture->LoadTextureData(GL_RGBA);
			loadedResources++;

			loadedTexturesList.push_back(texture);
			unloadedTexturesList.erase(unloadedTexturesList.begin() + i);
			i--;
		}
	}

	if (unloadedModelsList.size() == 0 && unloadedTexturesList.size() == 0)
	{
		isDirty = false;
	}
	
	int loadedTotal = loadedModelsList.size() + loadedTexturesList.size();
	int resourcesTotal = unloadedModelsList.size() + loadedModelsList.size() + unloadedTexturesList.size() + loadedTexturesList.size();
	this->loadingProgress = 90.f + ((float)loadedTotal / (float)resourcesTotal * 10.f);
}

void Scene::UnloadScene()
{
	for (size_t i = unloadedModelsList.size(); i >= unloadedModelsList.size(); i--)
	{
		unloadedModelsList[i]->DeleteBufferObjects();
		delete unloadedModelsList[i]; 
	}

	unloadedModelsList.clear(); 
	unloadedModelsList.shrink_to_fit(); 

	for (size_t i = unloadedTexturesList.size(); i >= unloadedTexturesList.size(); i--)
	{
		delete unloadedTexturesList[i]; 
	}

	unloadedTexturesList.clear(); 
	unloadedTexturesList.shrink_to_fit(); 

	for (size_t i = objectsList.size(); i >= objectsList.size(); i--) 
	{
		delete objectsList[i]; 
	}

	objectsList.clear(); 
	objectsList.shrink_to_fit(); 

	loadingProgress = 0.f;
	areResourcesStreamed = false;
	isDirty = true;

	isLoading = true;
	isActive = false;
}