#pragma once


#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <vector>

#include "SceneViewerClient.h"

#include "Scene.h"
#include "ShaderObject.h"
#include "LightObject.h"
#include "PerspectiveCameraObject.h"

class SceneManager
{
public:
	static SceneManager* getInstance();
	static void initialize(int nScenes, SceneViewerClient* client);

	Scene* GetSceneByID(int id);
	void OpenSingleScene(int id);
	void OpenAllScenes();
	void CloseAllScenes();
	
	bool AllScenesLoaded();
	
	void LoadSingleScene(int id);
	void UnloadSingleScene(int id);
	void UnloadAllScenes();
	void Draw(ShaderObject* shader, PerspectiveCameraObject camera, LightObject light);

	std::vector<Scene*> GetSceneList();

private:
	SceneManager(int nScenes, SceneViewerClient* client);
	~SceneManager();
	SceneManager(SceneManager const&) {};
	SceneManager& operator =(SceneManager const&) {};
	static SceneManager* instance;

	std::vector<Scene*> sceneList;

	SceneViewerClient* client;
};

