#pragma once
#include <vector>
#include "ModelObject.h"

class Scene
{
public:
	Scene(int id);
	~Scene();

	void UnloadScene();


public:
	int id;
	bool isAlreadyLoaded;
	float loadingProgress;

	std::vector<ModelObject*> modelsList;

};