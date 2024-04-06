#include "SceneViewerClient.h"
#include "multithreading/IETThread.h"
#include <map>

SceneViewerClient::SceneViewerClient(std::shared_ptr<grpc::Channel> channel) : stub(SceneLoader::NewStub(channel)), channel(channel)
{

}

bool SceneViewerClient::LoadModelsInScene(int id)
{
    // Get Scene
    auto sceneItr = std::find_if(scenesList.begin(), scenesList.end(), [&](const Scene* x) {
        return x->id == id;
        });

    Scene* currentScene = NULL;
    if (sceneItr != scenesList.end())
    {
        currentScene = *sceneItr;
    }
    else
    {
        std::cout << "SCENE(" << id << ") DOES NOT EXIST";
        OnRPCFail(currentScene);
        return false;
    }

    // Prepare request
    IntValue sceneID;
    sceneID.set_value(id);

    for (int i = 0; i < MAX_RETRIES; i++)
    {
        // Check connection
        if (!CheckConnectionToServer())
        {
            OnRPCFail(currentScene);
            return false;
        }

        // Do RPC call
        std::chrono::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(DEFAULT_STREAM_MODELS_DEADLINE);
        grpc::ClientContext context;
        context.set_deadline(deadline);
        std::unique_ptr<grpc::ClientReader<ModelData>> reader(stub->LoadModelsInScene(&context, sceneID));

        // Retrieve response
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

            auto modelItr = std::find_if(currentScene->unloadedModelsList.begin(), currentScene->unloadedModelsList.end(), [&](ModelObject* x) { 
                return x->GetModelName() == modelData.modelname(); 
                });

            if (modelItr != currentScene->unloadedModelsList.end()) 
            {
                ModelObject* ref = *modelItr; 
                ref->InsertPartialData(modelData.vdataindex(), vertexData);  
            }
            else
            {
                ModelObject* ref = new ModelObject(modelData.modelname()); 
                currentScene->unloadedModelsList.push_back(ref); 
                ref->InsertPartialData(modelData.vdataindex(), vertexData); 
            }

            currentScene->SetLoadingProgress(modelData.sceneprogress());
            //std::cout << currentScene->GetLoadingProgress() << "\n";
        }

        // Check status
        if (auto status = reader->Finish(); status.ok())
        {
            std::cout << "SUCCESS: Models created on Scene" << std::to_string(id) << "\n";
            return true;
        }
        else if (i < MAX_RETRIES - 1)
        {
            currentScene->UnloadModels();
            std::cout << "ATTEMPT " << i + 1 << " FAILED... RETRYING TO RELOAD MODELS ON SCENE " << std::to_string(id) << "\n";
            IETThread::sleep(RETRY_COOLDOWN);
        }
        else
        {
            std::cout << "ATTEMPT " << i + 1 << " FAILED... CANNOT LOAD MODELS FROM SERVER\n"
                << "CODE: " << status.error_code() << "   MESSAGE: " << status.error_message() << "   DETAILS: " << status.error_details() << "\n";

            OnRPCFail(currentScene);
            return false;
        }
    }
}

bool SceneViewerClient::LoadTexturesInScene(int id)
{
    // Get Scene
    auto sceneItr = std::find_if(scenesList.begin(), scenesList.end(), [&](const Scene* x) {
        return x->id == id;
        });

    Scene* currentScene = NULL;
    if (sceneItr != scenesList.end())
    {
        currentScene = *sceneItr;
    }
    else
    {
        std::cout << "SCENE(" << id << ") DOES NOT EXIST";
        OnRPCFail(currentScene);
        return false;
    }

    // Prepare request
    IntValue sceneID;
    sceneID.set_value(id);
    std::map<std::string, GLint> textureColorChannels; 

    for (int i = 0; i < MAX_RETRIES; i++)
    {
        // Check connection
        if (!CheckConnectionToServer())
        {
            OnRPCFail(currentScene);
            return false;
        }

        // Do RPC call
        std::chrono::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(DEFAULT_STREAM_TEXTURES_DEADLINE);
        grpc::ClientContext context;
        context.set_deadline(deadline);
        std::unique_ptr<grpc::ClientReader<TextureData>> reader(stub->LoadTexturesInScene(&context, sceneID));

        // Retrieve response
        TextureData texData;
        while (reader->Read(&texData))
        {
            std::string name = texData.texturename();
            float width = texData.width();
            float height = texData.height();
            GLint imageFormat = texData.hasalpha() ? GL_RGBA : GL_RGB;
            unsigned bytesPerPixel = texData.hasalpha() ? 4 : 3;
            int index = texData.pixelindex() * bytesPerPixel;

            auto textureItr = std::find_if(currentScene->unloadedModelsList.begin(), currentScene->unloadedModelsList.end(), [&](ModelObject* x) { 
                return x->GetModelName() == name; 
                });
             
            ModelObject* ref = NULL; 
            if (textureItr != currentScene->unloadedModelsList.end()) 
            {
                ref = *textureItr; 
            }
            /*else
            {
                ref = new Texture(name, width, height);
                currentScene->texturesList.push_back(ref);
            }*/

            for (int i = 0; i < texData.pixeldatabatch_size(); i++) 
            {
                PixelData pixel = texData.pixeldatabatch(i); 
                ref->InsertPartialTextureData(width, height, index + i * bytesPerPixel, bytesPerPixel, pixel.r(), pixel.g(), pixel.b(), pixel.a()); 
            }

            if (!textureColorChannels.contains(name)) 
            {
                textureColorChannels[name] = imageFormat; 
            }

            currentScene->SetLoadingProgress(texData.sceneprogress());
            //std::cout << currentScene->GetLoadingProgress() << "\n";
        }

        // Check status
        if (auto status = reader->Finish(); status.ok())
        {
            std::cout << "SUCCESS: Textures created on Scene" << std::to_string(id) << "\n";
            return true;
        }
        else if (i < MAX_RETRIES - 1)
        {
            currentScene->UnloadTextures();
            std::cout << "ATTEMPT " << i + 1 << " FAILED... RETRYING TO RELOAD TEXTURES ON SCENE " << std::to_string(id) << "\n";
            IETThread::sleep(RETRY_COOLDOWN);
        }
        else
        {
            std::cout << "ATTEMPT " << i + 1 << " FAILED... CANNOT LOAD TEXTURES FROM SERVER\n"
                << "CODE: " << status.error_code() << "   MESSAGE: " << status.error_message() << "   DETAILS: " << status.error_details() << "\n";

            OnRPCFail(currentScene);
            return false;
        }
    }
}

bool SceneViewerClient::LoadObjectsInScene(int id)
{
    // Get Scene
    auto sceneItr = std::find_if(scenesList.begin(), scenesList.end(), [&](const Scene* x) {
        return x->id == id;
        });

    Scene* currentScene = NULL;
    if (sceneItr != scenesList.end())
    {
        currentScene = *sceneItr;
    }
    else
    {
        std::cout << "SCENE(" << id << ") DOES NOT EXIST";
        OnRPCFail(currentScene);
        return false;
    }

    // Prepare request
    IntValue sceneID;
    sceneID.set_value(id);

    for (int i = 0; i < MAX_RETRIES; i++)
    {
        // Check connection
        if (!CheckConnectionToServer())
        {
            OnRPCFail(currentScene);
            return false;
        }

        // Do RPC call
        std::chrono::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds(DEFAULT_STREAM_OBJECTS_DEADLINE); 
        grpc::ClientContext context; 
        context.set_deadline(deadline); 
        std::unique_ptr<grpc::ClientReader<ObjectData>> reader(stub->LoadObjectsInScene(&context, sceneID)); 

        // Retrieve response
        ObjectData objData; 
        while (reader->Read(&objData)) 
        {
            auto modelItr = std::find_if(currentScene->unloadedModelsList.begin(), currentScene->unloadedModelsList.end(), [&](ModelObject* x) { 
                return x->GetModelName() == objData.modelname(); 
                });

            ModelObject* model = *modelItr; //Ref 

            ModelObject* modelGameObject = new ModelObject(model); 

            modelGameObject->SetPosition(glm::vec3(objData.position().x(), objData.position().y(), objData.position().z())); 
            modelGameObject->SetScale(glm::vec3(objData.scale().x(), objData.scale().y(), objData.scale().z())); 

            modelGameObject->ProcessPartialVertexData(); 

            currentScene->modelList.push_back(modelGameObject); 
            currentScene->SetLoadingProgress(objData.sceneprogress());
            //std::cout << currentScene->GetLoadingProgress() << "\n";
        }

        // Check status
        if (auto status = reader->Finish(); status.ok())
        {
            std::cout << "SUCCESS: Objects created on Scene" << std::to_string(id) << "\n";
            currentScene->areResourcesStreamed = true;
            return true;
        }
        else if (i < MAX_RETRIES - 1)
        {
            currentScene->UnloadObjects();
            std::cout << "ATTEMPT " << i + 1 << " FAILED... RETRYING TO RELOAD OBJECTS ON SCENE " << std::to_string(id) << "\n";
            IETThread::sleep(RETRY_COOLDOWN);
        }
        else
        {
            std::cout << "ATTEMPT " << i + 1 << " FAILED... CANNOT LOAD OBJECTS FROM SERVER\n"
                << "CODE: " << status.error_code() << "   MESSAGE: " << status.error_message() << "   DETAILS: " << status.error_details() << "\n";

            OnRPCFail(currentScene);
            return false;
        }
    }
}

bool SceneViewerClient::CheckConnectionToServer()
{
    if (auto connection = channel->GetState(true); connection == grpc_connectivity_state::GRPC_CHANNEL_IDLE)  
    {
        IETThread::sleep(CONNECT_TO_SERVER_DURATION); 
        //connection = channel->GetState(false); 
    }

    std::chrono::time_point tryConnectionDeadline = std::chrono::system_clock::now() + std::chrono::milliseconds(CONNECT_TO_SERVER_DURATION); 
    if (channel->WaitForConnected(tryConnectionDeadline)) 
    {
        return true;
    }
    else
    {
        std::cout << "CLIENT IS NOT CONNECTED TO THE SERVER - PLEASE RETRY CONNECTION\n";
        return false;
    }

    /*if (connection != grpc_connectivity_state::GRPC_CHANNEL_READY)
    {
        return false;
    }
    else
    {
        return true;
    }*/
}

void SceneViewerClient::OnRPCFail(Scene* currentScene)
{
    currentScene->loadingProgress = 0.f;
    currentScene->areResourcesStreamed = false;
    currentScene->isDirty = true;
}