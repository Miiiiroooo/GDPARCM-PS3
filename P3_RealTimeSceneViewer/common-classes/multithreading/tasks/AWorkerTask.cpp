#include "AWorkerTask.h"

AWorkerTask::AWorkerTask()
{

}

AWorkerTask::~AWorkerTask()
{
	listenersList.clear();
}

void AWorkerTask::AddFinishedTaskListener(IFinishedTaskListener* listener)
{
	listenersList.push_back(listener);
}

void AWorkerTask::NotifyListeners()
{
	for (size_t i = 0; i < listenersList.size(); i++)
	{
		listenersList[i]->OnFinishedTask();
	}
}