#pragma once
#include "IETThread.h"
#include "tasks/AWorkerTask.h"

class ThreadPoolScheduler;

class PoolWorker : public IETThread
{
public:
	PoolWorker(int id, ThreadPoolScheduler* threadPool);
	~PoolWorker();

	int GetID();
	void AssignTask(AWorkerTask* task);

protected:
	void run() override;


protected:
	int id;
	AWorkerTask* currentTask;
	ThreadPoolScheduler* threadPool;
};