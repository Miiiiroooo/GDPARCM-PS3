#include "PoolWorker.h"
#include "ThreadPoolScheduler.h"

PoolWorker::PoolWorker(int id, ThreadPoolScheduler* threadPool) : id(id), threadPool(threadPool), currentTask(NULL)
{

}

PoolWorker::~PoolWorker()
{
	if (currentTask != NULL)
	{
		delete currentTask;
		currentTask = NULL;
	}
}

int PoolWorker::GetID()
{
	return this->id; 
}

void PoolWorker::AssignTask(AWorkerTask* task)
{
	this->currentTask = task;
}

void PoolWorker::run()
{
	this->currentTask->ExecuteTask();

	threadPool->OnFinishedWorker(this->id);
}