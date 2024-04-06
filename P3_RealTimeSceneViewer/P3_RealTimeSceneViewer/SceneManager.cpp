#include "SceneManager.h"
#include "multithreading/ThreadPoolScheduler.h"
#include "LoadSceneTask.h"

#include <iostream>
#include <thread>

SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::getInstance()
{
	return instance;
}

void SceneManager::initialize(int nScenes, SceneViewerClient* client)
{
	instance = new SceneManager(nScenes, client);
}

Scene* SceneManager::GetSceneByID(int id)
{
	for (int i = 0; i < sceneList.size(); i++)
	{
		if (sceneList[i]->id == id)
		{
			return sceneList[i];
		}
	}

	std::cout << "Failed to Get Scene By ID" << std::endl;
}

void SceneManager::OpenSingleScene(int id)
{
	for (int i = 0; i < sceneList.size(); i++)
	{
		if (sceneList[i]->id == id)
		{
			sceneList[i]->isActive = true;
		}
		else
		{
			sceneList[i]->isActive = false;
		}
	}
}

void SceneManager::OpenAllScenes()
{
	for (int i = 0; i < sceneList.size(); i++)
	{
		sceneList[i]->isActive = true;
	}
}

void SceneManager::CloseAllScenes()
{
	for (int i = 0; i < sceneList.size(); i++)
	{
		sceneList[i]->isActive = false;
	}
}

bool SceneManager::AllScenesLoaded()
{
	for (int i = 0; i < sceneList.size(); i++)
	{
		if (!sceneList[i]->areResourcesStreamed)
		{
			return false;
		}
	}

	return true;
}

void SceneManager::LoadSingleScene(int id)
{
	for (int i = 0; i < sceneList.size(); i++)
	{
		if (sceneList[i]->id == id)
		{
			if (!sceneList[i]->areResourcesStreamed)
			{
				LoadSceneTask* task = new LoadSceneTask(sceneList[i]->id, client);
				ThreadPoolScheduler::GetInstance()->ScheduleTask(task);
				sceneList[i]->isLoading = true;
			}

		}
	}
}

void SceneManager::UnloadSingleScene(int id)
{
	for (int i = 0; i < sceneList.size(); i++)
	{
		if (sceneList[i]->id == id)
		{
			if (sceneList[i]->areResourcesStreamed)
			{
				sceneList[i]->UnloadScene();
			}
			
		}
	}
}

void SceneManager::UnloadAllScenes()
{
	for (int i = 0; i < sceneList.size(); i++)
	{
		sceneList[i]->UnloadScene();
	}

	for (auto scene : sceneList)
	{
		LoadSceneTask* task = new LoadSceneTask(scene->id, client);
		ThreadPoolScheduler::GetInstance()->ScheduleTask(task);
	}
}

void SceneManager::Draw(ShaderObject* shader, PerspectiveCameraObject camera, LightObject light)
{
	for (int i = 0; i < sceneList.size(); i++)
	{
		Scene* scene = sceneList[i];

		if (scene->areResourcesStreamed)
		{

			if (scene->isDirty)
			{
				scene->LoadAllResourcesToOpenGL();
			}
			
			if (scene->isActive)
			{
				for (int j = 0; j < scene->modelList.size(); j++)
				{
					shader->UseShader();
					scene->modelList[j]->Draw(shader->GetShaderProgram(), camera, light);
				}
			}
			
		}
	}
}

std::vector<Scene*> SceneManager::GetSceneList()
{
	return sceneList;
}

SceneManager::SceneManager(int nScenes, SceneViewerClient* client)
{
	this->client = client;

	for (int i = 1; i < nScenes + 1; i++)
	{
		std::cout << "Created new scene id: " << i << std::endl;
		sceneList.push_back(new Scene(i));
	
	}

}

SceneManager::~SceneManager()
{
	delete instance;
}
