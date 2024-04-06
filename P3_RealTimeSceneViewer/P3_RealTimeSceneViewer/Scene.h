#pragma once
#include <vector>
#include "ModelObject.h"

class Scene
{
public:
	Scene(int id);
	~Scene();

	float GetLoadingProgress();
	void SetLoadingProgress(float value);

	void LoadAllResourcesToOpenGL();
	void UnloadScene();


public:
	int id;
	bool areResourcesStreamed;
	bool isDirty;
	float loadingProgress;

	bool isActive;
	bool isLoading;

	std::vector<ModelObject*> unloadedModelsList;
	std::vector<ModelObject*> modelList;

};