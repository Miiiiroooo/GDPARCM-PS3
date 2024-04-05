#pragma once
#include "multithreading/tasks/AWorkerTask.h"
#include "SceneViewerClient.h"

class LoadSceneTask : public AWorkerTask
{
public:
	LoadSceneTask(int sceneID, SceneViewerClient* client);
	~LoadSceneTask();

	void ExecuteTask() override;


private:
	int sceneID;
	SceneViewerClient* client;
};