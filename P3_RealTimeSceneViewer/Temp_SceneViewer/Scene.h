#pragma once
#include <vector>
#include <semaphore>
#include "opengl/Model3D.h"

class Scene
{
public:
	Scene(int id);
	~Scene();

	float GetLoadingProgress();
	void SetLoadingProgress(float value);

	void LoadAllResourcesToOpenGL();
	void UnloadModels();
	void UnloadTextures();
	void UnloadObjects();
	void UnloadScene();


public:
	int id;
	bool areResourcesStreamed;
	bool isDirty;

	bool isActive;
	bool isLoading;

	const int MAX_SETUP_BUFFERS_IN_MAIN = 3;

	std::vector<ModelReference*> unloadedModelsList;
	std::vector<ModelReference*> loadedModelsList;
	std::vector<Texture*> unloadedTexturesList;
	std::vector<Texture*> loadedTexturesList;
	std::vector<Model3D*> objectsList;

private:
	float loadingProgress;
	std::binary_semaphore* progressSem;
};