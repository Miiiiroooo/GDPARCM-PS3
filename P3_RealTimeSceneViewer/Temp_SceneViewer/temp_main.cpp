#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include"../../common-classes/stb_image.h"

#include "../common-classes/opengl/Shader.h"
#include "../common-classes/opengl/Model3D.h"
#include "../common-classes/opengl/Texture.h"
#include "../common-classes/opengl/DirectionalLight.h"
#include "../common-classes/opengl/OrthoCamera.h"
#include "../common-classes/opengl/PerspectiveCamera.h"
#include "../common-classes/Utils.h"

#include <grpcpp/grpcpp.h>
#include "../proto/SceneLoader.grpc.pb.h"


#pragma region Global Variables
// Shaders, Models, Textures, Lights, Cameras, the Skybox, and the Player
std::vector<Shader*> shadersList;
std::vector<ModelReference*> modelReferencesList;
std::vector<Model3D*> modelsList;
std::vector<Texture*> texturesList;
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


#pragma region Method Declarations
bool InitializeOpenGL(GLFWwindow** window);
void LoadShaders(const std::vector<std::pair<std::string, std::string>>& shaderPathsList);
void LoadObjects(const std::vector<std::string>& objPathsList);
void LoadTextures(const std::vector<std::pair<std::string, GLuint>>& textureInfoList);
void CreateModels(const std::vector<std::pair<int, int>>& objTextureMap, const std::vector<glm::vec3>& modelsInfoList);
void CreateLights(const std::vector<std::pair<glm::vec3, unsigned int>>& lightsInfoList);
void CreateCameras(const std::vector<glm::vec3>& camerasInfoList);

glm::mat4 CreateViewMatrix();
void ComputeVerticesWithShaders(GLuint& shaderProgram, glm::mat4& transformationMatrix, glm::mat4& viewMatrix);
void ComputeFragmentsWithShaders(GLuint& shaderProgram, Texture* texture, glm::vec3& color);
void PassLightingData(GLuint& shaderProgram);
#pragma endregion


class SceneViewer
{
public: 
    SceneViewer(std::shared_ptr< grpc::Channel> channel) : stub(SceneLoader::NewStub(channel))
    {

    }

    void LoadTexturesInScene(int id)
    {
        GLubyte* pixels = NULL;
        GLint imageFormat;
        int width;
        int height; 

        
        IntValue sceneID; 
        sceneID.set_value(id);

        grpc::ClientContext context;  
        std::unique_ptr<grpc::ClientReader<TextureData>> reader(stub->LoadTexturesInScene(&context, sceneID)); 

        TextureData texData;
        while (reader->Read(&texData))
        {
            width = texData.width(); 
            height = texData.height(); 
            imageFormat = texData.hasalpha() ? GL_RGBA : GL_RGB; 
            unsigned bytePerPixel = texData.hasalpha() ? 4 : 3; 
            int index = texData.pixelindex() * bytePerPixel;

            if (pixels == NULL) 
            {
                pixels = new GLubyte[width * height * bytePerPixel]; 
            }

            pixels[index] = texData.pixeldata().r();
            pixels[index+1] = texData.pixeldata().g(); 
            pixels[index+2] = texData.pixeldata().b();
            pixels[index+3] = texData.pixeldata().a(); 
        }

        grpc::Status status = reader->Finish();  
        if (status.ok()) 
        {
            Texture* newTexture = new Texture("");  
            newTexture->LoadTextureData(texData.width(), texData.height(), imageFormat, pixels);  
            texturesList.push_back(newTexture);  

            std::cout << "TEXTURE CREATED\n"; 
        }
        else
        {
            std::cout << "FAIL TO RECEIVE TEXTURE " << status.error_code() << " " << status.error_message() << " " << status.error_details() << "\n"; 
        }
    }

    void LoadObjectsInScene(int id)
    {
        std::map<int, std::vector<float>> modelDataMap; 

        IntValue sceneID;
        sceneID.set_value(id);

        grpc::ClientContext context;
        std::unique_ptr<grpc::ClientReader<ModelData>> reader(stub->LoadModelsInScene(&context, sceneID)); 

        ModelData modelData;  
        while (reader->Read(&modelData)) 
        {
            std::vector<float> vertexData = { 
                modelData.vdata().position().x(),
                modelData.vdata().position().y(), 
                modelData.vdata().position().z(),
                modelData.vdata().normals().x(),
                modelData.vdata().normals().y(), 
                modelData.vdata().normals().z(), 
                modelData.vdata().u(), 
                modelData.vdata().v() 
            };

            modelDataMap[modelData.vdataindex()] = vertexData;
        }

        grpc::Status status = reader->Finish();
        if (status.ok())
        {
            ModelReference* ref = new ModelReference(""); 
            std::vector<float> fullVertexData;

            for (auto pair : modelDataMap)
            {
                fullVertexData.insert(fullVertexData.end(), pair.second.begin(), pair.second.end());
            }

            ref->LoadModelData(fullVertexData);
            modelReferencesList.push_back(ref);
            
            Model3D* model = new Model3D(ref, texturesList.front());
            model->transform.position = glm::vec3(0.f, 0.f, 10.f);
            model->transform.Rotate(glm::vec3(0.f, 180.f, 0.f));
            model->transform.scale = glm::vec3(0.05f, 0.05f, 0.05f);
            modelsList.push_back(model);

            std::cout << "SUCCESS MODEL CREATED\n";
        }
        else
        {
            std::cout << "FAIL TO RECEVIVE MODEL " << status.error_code() << " " << status.error_message()  << " " << status.error_details() << "\n";
        }
    }


private:
    std::unique_ptr<SceneLoader::Stub> stub;
};


int main(void)
{
    // Initialize program
    srand(time(0));
    GLFWwindow* window;
    if (!InitializeOpenGL(&window))
        return -1;

    // Load shaders 
    std::vector<std::pair<std::string, std::string>> shaderPathsList = {
        {"Shaders/default.vert", "Shaders/default.frag"},
    };
    LoadShaders(shaderPathsList);


    //// Load 3d object references
    //std::vector<std::string> objPathsList = {
    //    "../3D/amumu.obj",
    //};
    //LoadObjects(objPathsList);

    //// Load textures
    //std::vector<std::pair<std::string, GLuint>> textureInfoList = { 
    //    {"../3D/amumu.png", GL_RGBA}, 
    //};
    //LoadTextures(textureInfoList); 

    //// Create new models
    //std::vector<std::pair<int, int>> objTextureMap = {
    //    {0, 0},
    //};
    //std::vector<glm::vec3> modelsInfoList = {
    //    // amumu
    //    glm::vec3(0.f, 0.f, 10.f),
    //    glm::vec3(0.f, 180.f, 0.f),
    //    glm::vec3(0.05f, 0.05f, 0.05f),
    //    glm::vec3(1.f, 1.f, 1.f),
    //};
    //CreateModels(objTextureMap, modelsInfoList);

    std::string server_adr = "localhost:50052";
    SceneViewer viewer(grpc::CreateChannel(server_adr, grpc::InsecureChannelCredentials()));
    viewer.LoadTexturesInScene(0); 
    viewer.LoadObjectsInScene(0);


    // Create Lights
    std::vector<std::pair<glm::vec3, unsigned int>> lightsInfoList = {
        {glm::vec3(0.f, 0.f, 0.f), 0},              // position, light type
    };
    CreateLights(lightsInfoList);


    // Create Cameras
    std::vector<glm::vec3> camerasInfoList = {
        // first-person
        glm::vec3(0.f, 0.f, 0.f),
        glm::vec3(0.f, 0.f, 0.f),
        modelsList[0]->transform.localForward,
    };
    CreateCameras(camerasInfoList); 


    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Compute deltaTime
        timeSinceStart = glfwGetTime();
        deltaTime = timeSinceStart - oldTimeSinceStart;
        oldTimeSinceStart = timeSinceStart;


        // Iterate through modelsList
        for (Model3D* model : modelsList)
        {
            glm::mat4 transformationMatrix = model->GetTransformationMatrix();
            glm::mat4 viewMatrix = CreateViewMatrix();

            ComputeVerticesWithShaders(shadersList[0]->GetShaderProgram(), transformationMatrix, viewMatrix);
            ComputeFragmentsWithShaders(shadersList[0]->GetShaderProgram(), model->texture, model->color);

            glUseProgram(shadersList[0]->GetShaderProgram());
            glBindVertexArray(model->objRef->GetVAO());
            glDrawArrays(GL_TRIANGLES, 0, model->objRef->GetFullVertexData().size() / 8);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (Model3D* model : modelsList)
    {
        model->objRef->DeleteBufferObjects();
    }
    glfwTerminate();

    return 0;
}


#pragma region Initialize Program
bool InitializeOpenGL(GLFWwindow** window)
{
    if (!glfwInit())
        return false;

    *window = glfwCreateWindow(width, height, "Aamir Akim", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(*window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    return true;
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

void LoadObjects(const std::vector<std::string>& objPathsList)
{
    for (std::string path : objPathsList)
    {
        ModelReference* reference = new ModelReference(path);
        reference->LoadModel();
        modelReferencesList.push_back(reference);
    }
}

void LoadTextures(const std::vector<std::pair<std::string, GLuint>>& textureInfoList)
{
    for (std::pair<std::string, GLint> info : textureInfoList)
    {
        Texture* texture = new Texture(info.first.c_str()); // pass in file path
        texture->LoadTexture(info.second);                  // pass in image format
        texturesList.push_back(texture);
    }
}

void CreateModels(const std::vector<std::pair<int, int>>& objTextureMap, const std::vector<glm::vec3>& modelsInfoList)
{
    for (int i = 0; i < objTextureMap.size(); i++)
    {
        std::pair<int, int> pair = objTextureMap[i];     // first - index to obj ref, second - index to texture

        ModelReference* objRef = modelReferencesList[pair.first];
        Texture* texture = (pair.second != -1) ? texturesList[pair.second] : NULL;

        Model3D* newModel = new Model3D(objRef, texture);
        modelsList.push_back(newModel);

        newModel->transform.position = modelsInfoList[i * 4 + 0];
        newModel->transform.Rotate(modelsInfoList[i * 4 + 1]);
        newModel->transform.scale = modelsInfoList[i * 4 + 2];
        newModel->color = modelsInfoList[i * 4 + 3];
    }
}

void CreateLights(const std::vector<std::pair<glm::vec3, unsigned int>>& lightsInfoList)
{
    for (std::pair<glm::vec3, unsigned int> info : lightsInfoList)   // first - light pos, second - light type
    {
        Light* light;

        if (info.second == 0)
        {
            DirectionalLight* dirLight = new DirectionalLight();
            dirLight->transform.position = info.first;
            dirLight->lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            dirLight->ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
            dirLight->lightIntensity = 3.f;

            glm::vec3 dir = glm::vec3(0.25f, -1.f, -0.5f);
            dir = glm::normalize(dir);
            dirLight->lightDir = dir;

            light = dirLight;
        }

        lightsList.push_back(light);
    }
}

void CreateCameras(const std::vector<glm::vec3>& camerasInfoList)
{
    PerspectiveCamera* firstPerson = new PerspectiveCamera(fov, width, height, 0.6f, 100.f);
    firstPerson->ComputeProjectionMatrix();
    firstPerson->transform.position = camerasInfoList[0];
    firstPerson->transform.Rotate(camerasInfoList[1]);
    firstPerson->SetCamTarget(camerasInfoList[2]);
    camerasList.push_back(firstPerson);

    mainCamera = camerasList[0];
}
#pragma endregion

#pragma region Rendering
glm::mat4 CreateViewMatrix()
{
    glm::mat4 camPosMatrix = glm::translate(Utils::IDENTITY_MAT4, -mainCamera->transform.position);
    glm::vec3 F = glm::normalize(mainCamera->transform.localForward);
    glm::vec3 R = glm::normalize(glm::cross(Utils::WORLD_UP, F)); 
    glm::vec3 U = glm::normalize(glm::cross(F, R));

    glm::mat4 camOrientation = glm::mat4(1.0f);
    camOrientation[0][0] = R.x;
    camOrientation[1][0] = R.y;
    camOrientation[2][0] = R.z;
    camOrientation[0][1] = U.x;
    camOrientation[1][1] = U.y;
    camOrientation[2][1] = U.z;
    camOrientation[0][2] = -F.x;
    camOrientation[1][2] = -F.y;
    camOrientation[2][2] = -F.z;

    return camOrientation * camPosMatrix;
}

void ComputeVerticesWithShaders(GLuint& shaderProgram, glm::mat4& transformationMatrix, glm::mat4& viewMatrix)
{
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformationMatrix));

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(mainCamera->GetProjectionMatrix()));
}

void ComputeFragmentsWithShaders(GLuint& shaderProgram, Texture* texture, glm::vec3& color)
{
    GLuint isUntlitAddress = glGetUniformLocation(shaderProgram, "isUntlit");
    glUniform1i(isUntlitAddress, (int)(texture == NULL));

    unsigned int colorShader = glGetUniformLocation(shaderProgram, "newColor");
    glUniform3fv(colorShader, 1, glm::value_ptr(color));

    if (texture != NULL)
    {
        GLuint tex0Address = glGetUniformLocation(shaderProgram, "tex0");
        glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
        glUniform1i(tex0Address, 0);
    }

    GLuint isNightVisionAddress = glGetUniformLocation(shaderProgram, "isNightVision");
    glUniform1i(isNightVisionAddress, (int)(false));

    GLuint camPosAddress = glGetUniformLocation(shaderProgram, "camPos");
    glUniform3fv(camPosAddress, 1, glm::value_ptr(mainCamera->transform.position));


    PassLightingData(shaderProgram);
}

void PassLightingData(GLuint& shaderProgram)
{
    for (Light* light : lightsList)
    {
        if (Utils::InstanceOf<Light, DirectionalLight>(light))
        {
            DirectionalLight* dirLight = Utils::ConvertTo<Light, DirectionalLight>(light);

            GLuint dirAddress = glGetUniformLocation(shaderProgram, "dirLight.dir");
            glUniform3fv(dirAddress, 1, glm::value_ptr(dirLight->lightDir));
            GLuint colorAddress = glGetUniformLocation(shaderProgram, "dirLight.color");
            glUniform3fv(colorAddress, 1, glm::value_ptr(dirLight->lightColor));
            GLuint intensityAddress = glGetUniformLocation(shaderProgram, "dirLight.intensity");
            glUniform1f(intensityAddress, dirLight->lightIntensity);

            GLuint ambientStrAddress = glGetUniformLocation(shaderProgram, "dirLight.ambientStr");
            glUniform1f(ambientStrAddress, dirLight->ambientStr);
            GLuint ambientColorAddress = glGetUniformLocation(shaderProgram, "dirLight.ambientColor");
            glUniform3fv(ambientColorAddress, 1, glm::value_ptr(dirLight->ambientColor));

            GLuint specStrAddress = glGetUniformLocation(shaderProgram, "dirLight.specStr");
            glUniform1f(specStrAddress, dirLight->specStr);
            GLuint specPhongAddress = glGetUniformLocation(shaderProgram, "dirLight.specPhong");
            glUniform1f(specPhongAddress, dirLight->specPhong);
        }
    }
}
#pragma endregion