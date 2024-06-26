#pragma once
#include"AUIPanel.h"
#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>



//DearIMGUI Includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>

class Scene;

class MainMenuPanel : public AUIPanel
{
public:
	MainMenuPanel(std::string name);
	~MainMenuPanel() {};

	void draw() override;

private:
	std::vector<GLuint> sceneThumbnails;
	std::vector<GLuint> sceneSelectedThumbnails;
	GLuint loadTexture(const char* path);

	std::vector<Scene*> savedScenes;
};

