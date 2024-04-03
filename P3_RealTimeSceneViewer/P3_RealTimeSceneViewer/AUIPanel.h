#pragma once
#include<string>
#include"imgui.h"


class AUIPanel
{
public:
	AUIPanel(std::string name);
	~AUIPanel() {};

	std::string getName();
	virtual void draw() = 0;
	bool mIsEnabled = true;

private:

	std::string mPanelName;


};

