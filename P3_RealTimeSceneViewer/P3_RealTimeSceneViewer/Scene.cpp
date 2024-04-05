#include "Scene.h"

Scene::Scene(int id) : id(id)
{
	isAlreadyLoaded = false;
	loadingProgress = 0.f;
}

Scene::~Scene()
{
	UnloadScene();
}

void Scene::UnloadScene()
{
	for (size_t i = 0; i < modelsList.size(); i++)
	{
		delete modelsList[i];
	}

	modelsList.clear();
	modelsList.shrink_to_fit();

}