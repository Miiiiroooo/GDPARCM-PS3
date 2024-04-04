#pragma once
#include <vector>
#include "opengl/Model3D.h"

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

	std::vector<ModelReference*> modelsList;
	std::vector<Texture*> texturesList;
	std::vector<Model3D*> objectsList;
};