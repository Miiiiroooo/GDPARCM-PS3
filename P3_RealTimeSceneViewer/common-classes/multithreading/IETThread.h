#pragma once
#include <iostream>
#include <thread>

class IETThread
{
public:
	IETThread();
	virtual ~IETThread();

	void start();
	static void sleep(int ms);

protected:
	virtual void run() = 0;
};