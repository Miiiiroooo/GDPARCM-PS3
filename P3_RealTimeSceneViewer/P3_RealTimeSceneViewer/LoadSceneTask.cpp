#include "LoadSceneTask.h"

LoadSceneTask::LoadSceneTask(int sceneID, SceneViewerClient* client) : sceneID(sceneID), client(client)
{

}

LoadSceneTask::~LoadSceneTask()
{

}

void LoadSceneTask::ExecuteTask()
{
	if (client->LoadModelsInScene(sceneID) && client->LoadTexturesInScene(sceneID) && client->LoadObjectsInScene(sceneID))
	{
		std::cout << "ALL RESOURCES IN SCENE " << sceneID << " HAVE BEEN STREAMED PROPERLY";
	}

	delete this;
}