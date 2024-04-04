#include "ThreadPoolScheduler.h"
#include <iostream>


ThreadPoolScheduler* ThreadPoolScheduler::sharedInstance = NULL;

ThreadPoolScheduler* ThreadPoolScheduler::GetInstance()
{
	if (sharedInstance == NULL)
	{
		sharedInstance = new ThreadPoolScheduler();
	}

	return sharedInstance;
}

ThreadPoolScheduler::~ThreadPoolScheduler()
{
	StopScheduler();

	for (auto& element : activeThreads)
	{
		delete element.second;
		activeThreads.erase(element.first);
	}
	activeThreads.clear();

	while (inactiveThreads.size() > 0)
	{
		PoolWorker* worker = inactiveThreads.front();
		delete worker;
		inactiveThreads.pop();
	}

	while (pendingTasks.size() > 0)
	{
		AWorkerTask* task = pendingTasks.front(); 
		delete task;
		pendingTasks.pop();
	}
}

void ThreadPoolScheduler::Initialize(int reduceThreads)
{
	maxWorkers = std::thread::hardware_concurrency() * 2 - reduceThreads;
	isRunning = false;

	for (int i = 0; i < maxWorkers; i++)
	{
		PoolWorker* newWorker = new PoolWorker(i, this);
		inactiveThreads.push(newWorker);
	}
}

void ThreadPoolScheduler::StartScheduler()
{
	if (inactiveThreads.size() <= 0)
	{
		std::cout << "cannot start scheduler without initializing it\n";
		return;
	}

	isRunning = true;
}

void ThreadPoolScheduler::StopScheduler()
{
	isRunning = false;
}

void ThreadPoolScheduler::ScheduleTask(AWorkerTask* task)
{
	pendingTasks.push(task);
}

void ThreadPoolScheduler::OnFinishedWorker(int id)
{
	if (activeThreads[id] != NULL) 
	{
		PoolWorker* worker = activeThreads[id];
		activeThreads.erase(id);
		inactiveThreads.push(worker);

		/*delete activeThreads[id]; 
		activeThreads.erase(id); 

		PoolWorker* newWorker = new PoolWorker(id, this);
		inactiveThreads.push(newWorker); */
	}
}

// behaves similarly to an update function
void ThreadPoolScheduler::run()
{
	while (isRunning) 
	{
		//std::cout << "Current Tasks: " << pendingTasks.size() << "  Inactive Threads: " << inactiveThreads.size() << "\n";

		if (pendingTasks.size() > 0) 
		{
			if (inactiveThreads.size() > 0) 
			{
				PoolWorker* worker = inactiveThreads.front(); 
				AWorkerTask* task = pendingTasks.front(); 
				inactiveThreads.pop(); 
				pendingTasks.pop(); 

				worker->AssignTask(task);  
				worker->start();  
				activeThreads[worker->GetID()] = worker; 
			}
		}
	}
}