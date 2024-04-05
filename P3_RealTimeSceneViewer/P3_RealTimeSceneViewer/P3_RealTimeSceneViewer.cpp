
//OpenGL Includes
#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

//DearIMGUI Includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//Base Includes
#include <iostream>
#include<unordered_map>

//GRPC
#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"

//Created Classes Includes
#include "UIManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "SceneViewerClient.h"
#include "ModelObject.h"
#include "LightObject.h"
#include "PerspectiveCameraObject.h"
#include "ShaderObject.h"

//Threading
#include "multithreading/ThreadPoolScheduler.h"
#include "LoadSceneTask.h"

//UI
#include "MainMenuPanel.h"
#include "EngineProfiler.h"

//Definitions
#define TINYOBJLOADER_IMPLEMENTATION
#include"tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

//Forward Declarations
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//Global Variables
unordered_map<string, bool> input;
bool firstMouse = true;
double lastX, lastY;
glm::vec2 mousePos;

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

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    //Init Inputs
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    input["W"] = false;
    input["A"] = false;
    input["S"] = false;
    input["D"] = false;
    input["Space"] = false;
    input["LShift"] = false;
    input["mousePressed"] = false;

    mousePos.x = 270;

#pragma endregion

    //Time
    float delta = 0;
    float lastTime = glfwGetTime();

    //UI
    MainMenuPanel* scenePanel = new MainMenuPanel("ScenePanel");
    EngineProfiler* profiler = new EngineProfiler("Engine Profiler");
    //UIManager::getInstance()->initialize(window);

    //GameObject Declarations
    PerspectiveCameraObject camera;
    ShaderObject* shader = new ShaderObject("Shaders/shader.vert", "Shaders/shader.frag");

    LightObject light;
    light.SetLightPosition(glm::vec3(0, 5, 5));
    light.SetLightBrightness(100);
    light.SetLightColor(1.0f, 1.0f, 1.0f);


    //Scenes & Client
    /*Scene* scene1 = new Scene(1);

    std::string server_adr = "localhost:50052";
    SceneViewerClient client(grpc::CreateChannel(server_adr, grpc::InsecureChannelCredentials()));
    client.scenesList.push_back(scene1);
    client.LoadModelsInScene(1);
    client.LoadTexturesInScene(1);
    client.LoadObjectsInScene(1);*/

    /*std::vector<Scene*> scenesList = {
       new Scene(1),
       new Scene(2)
    };*/

  

    ThreadPoolScheduler::GetInstance()->Initialize(2);
    ThreadPoolScheduler::GetInstance()->StartScheduler();
    ThreadPoolScheduler::GetInstance()->start();

    std::string server_adr = "localhost:50052";
    SceneViewerClient client(grpc::CreateChannel(server_adr, grpc::InsecureChannelCredentials()));

    SceneManager::initialize(2, &client);

    for (auto scene : SceneManager::getInstance()->GetSceneList())
    {
        client.scenesList.push_back(scene);
        LoadSceneTask* task = new LoadSceneTask(scene->id, &client);
        ThreadPoolScheduler::GetInstance()->ScheduleTask(task);
    }
    


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
        camera.CameraMovement(input, mousePos, delta);

        /* Update */
        //UIManager::getInstance()->draw();
        profiler->UpdateFPS(delta);

        /* Draw */
        scenePanel->draw();
        profiler->draw();

        SceneManager::getInstance()->Draw(shader, camera, light);
 

        /*End of Loop*/
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        lastTime = currTime; // delta time
    }
    
    SceneManager::getInstance()->UnloadAllScenes();

    glfwDestroyWindow(window);
    glfwTerminate();

}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        input["W"] = true;
    }
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        input["W"] = false;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        input["A"] = true;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        input["A"] = false;
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        input["S"] = true;
    }
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        input["S"] = false;
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        input["D"] = true;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        input["D"] = false;
    }

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
    {
        input["LShift"] = true;
    }
    else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
    {
        input["LShift"] = false;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        input["Space"] = true;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        input["Space"] = false;
    }

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        input["mousePressed"] = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        input["mousePressed"] = false;
        firstMouse = true;
    }

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

    if (input["mousePressed"] == true)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.3f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        mousePos.x += xoffset;
        mousePos.y += yoffset;

    }

}
