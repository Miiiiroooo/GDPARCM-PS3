
//OpenGL Includes
#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include"tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//DearIMGUI Includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//Base Includes
#include <iostream>

//Created Classes Includes
#include "opengl/PerspectiveCamera.h"
#include "opengl/Shader.h"
#include "opengl/Light.h"
#include "UIManager.h"

//UI
#include "MainMenuPanel.h"
#include "EngineProfiler.h"

using namespace std;

#pragma region Global Variables
// Shaders, Models, Textures, Lights, Cameras, the Skybox, and the Player
std::vector<Shader*> shadersList;
std::vector<Light*> lightsList;
std::vector<Camera*> camerasList;
Camera* mainCamera;

// Time
float timeSinceStart = 0.f;
float oldTimeSinceStart = 0.f;
float deltaTime = 0.f;

// Projection
float fov = 90.f;
const float width = 600;
const float height = 600;
#pragma endregion

//Function Declarations
void LoadShaders(const std::vector<std::pair<std::string, std::string>>& shaderPathsList);

int main()
{
#pragma region GLFW Init
    float width = 800;
    float height = 800;
    GLFWwindow* window;


    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Real Time Scene Viewer - Macuha & Akim", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    glViewport(0, 0, width, height);

    /*
    * Needed for User Inputs
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    */

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

#pragma endregion

    //Time
    float delta = 0;
    float lastTime = glfwGetTime();


    //UI
    MainMenuPanel* scenePanel = new MainMenuPanel("ScenePanel");
    EngineProfiler* profiler = new EngineProfiler("Engine Profiler");
    //UIManager::getInstance()->initialize(window);

    //GameObject Declarations
    PerspectiveCamera camera;

    // Load shaders 
    std::vector<std::pair<std::string, std::string>> shaderPathsList = {
        {"Shaders/default.vert", "Shaders/default.frag"},
    };
    LoadShaders(shaderPathsList);


    //Main Loop
    while (!glfwWindowShouldClose(window))
    {
        /*Calculate Delta Time*/
        GLfloat currTime = glfwGetTime();
        delta = currTime - lastTime;


        /* Pre Start of Loop */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /*Start of Loop*/

        /* Update */
        //UIManager::getInstance()->draw();
        profiler->UpdateFPS(delta);

        /* Draw */
        scenePanel->draw();
        profiler->draw();


        /*End of Loop*/
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        lastTime = currTime; // delta time
    }
    
    /*On ShutDown*/
    //UIManager::getInstance()->destroy();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void LoadShaders(const std::vector<std::pair<std::string, std::string>>& shaderPathsList)
{
    for (std::pair<std::string, std::string> pair : shaderPathsList)
    {
        std::string vert = pair.first;
        std::string frag = pair.second;

        Shader* shader = new Shader();
        shader->InitializeProgram(vert, frag);
        shadersList.push_back(shader);
    }
}

