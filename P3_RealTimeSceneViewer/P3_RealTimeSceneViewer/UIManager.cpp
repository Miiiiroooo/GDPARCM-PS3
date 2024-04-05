#include "UIManager.h"
#include <iostream>

//Panels
#include "MainMenuPanel.h"
#include "EngineProfiler.h"


UIManager* UIManager::instance = nullptr;

UIManager* UIManager::getInstance()
{
	return nullptr;
}

void UIManager::initialize(GLFWwindow* window)
{
	instance = new UIManager(window);
}

void UIManager::destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UIManager::draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	for (int i = 0; i < instance->panels.size(); i++)
	{
		instance->panels[i]->draw();
	}
	

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



AUIPanel* UIManager::getPanel(PanelName panel_name)
{
	int index = panel_name;
	if (index >= 0 && index < panels.size()) return panels[index];
	else return nullptr;

	return nullptr;
}

void UIManager::setActive(AUIPanel* panel, bool is_enabled)
{
	panel->mIsEnabled = is_enabled;
}

UIManager::UIManager(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	MainMenuPanel* mainPanel = new MainMenuPanel("MainMenuPanel");
	panels.push_back(mainPanel);

	EngineProfiler* engineProfiler = new EngineProfiler("EngineProfiler");
	panels.push_back(engineProfiler);

}

UIManager::~UIManager() 
{
    delete instance;
}