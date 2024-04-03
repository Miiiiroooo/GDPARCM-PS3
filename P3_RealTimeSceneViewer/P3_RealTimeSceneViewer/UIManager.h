#pragma once
#pragma once
#include"AUIPanel.h"
#include<vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "MainMenuPanel.h"

class UIManager
{
public:
	enum PanelName {
		ScenePanel,
		EnginePanel
	};

	static UIManager* getInstance();
	static void initialize(GLFWwindow* window);
	static void destroy();

	void draw();

	AUIPanel* getPanel(PanelName panel_name);
	void setActive(AUIPanel* panel, bool is_enabled);

private:
	UIManager(GLFWwindow* window);
	~UIManager();
	UIManager(UIManager const&) {};
	UIManager& operator =(UIManager const&) {};
	static UIManager* instance;

	std::vector<AUIPanel*> panels;
};

