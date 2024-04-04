#pragma once
#include "multithreading/IETThread.h"

class SenderThread : public IETThread
{
public:
	SenderThread(int id);
	~SenderThread();

	int id;
	bool isRunning = false;

private:
	void run();

};

