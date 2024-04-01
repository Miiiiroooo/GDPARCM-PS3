#pragma once
#include "../IFinishedTaskListener.h"
#include <vector>


class AWorkerTask
{
public:
	AWorkerTask();
	~AWorkerTask();

	virtual void ExecuteTask() = 0;
	void AddFinishedTaskListener(IFinishedTaskListener* listener);

protected:
	void NotifyListeners();


private:
	std::vector<IFinishedTaskListener*>  listenersList;
};