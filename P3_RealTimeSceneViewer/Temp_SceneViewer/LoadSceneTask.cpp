#include "LoadSceneTask.h"

LoadSceneTask::LoadSceneTask(int sceneID, SceneViewerClient* client) : sceneID(sceneID), client(client)
{

}

LoadSceneTask::~LoadSceneTask()
{

}

void LoadSceneTask::ExecuteTask()
{
	client->LoadModelsInScene(sceneID);
	client->LoadTexturesInScene(sceneID);
	client->LoadObjectsInScene(sceneID);

	delete this;
}