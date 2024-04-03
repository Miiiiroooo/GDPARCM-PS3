
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
#include "UIManager.h"

//UI
#include "MainMenuPanel.h"
#include "EngineProfiler.h"

using namespace std;

//User Input (idk if needed)
//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);

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

