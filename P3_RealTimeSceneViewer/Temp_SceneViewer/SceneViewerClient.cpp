#include "SceneViewerClient.h"
#include <map>

SceneViewerClient::SceneViewerClient(std::shared_ptr<grpc::Channel> channel) : stub(SceneLoader::NewStub(channel))
{

}

void SceneViewerClient::LoadModelsInScene(int id)
{
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
        return;
    }


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

        auto modelItr = std::find_if(currentScene->modelsList.begin(), currentScene->modelsList.end(), [&](ModelReference* x) {
            return x->GetModelName() == modelData.modelname();
            });
        
        if (modelItr != currentScene->modelsList.end())
        {
            ModelReference* ref = *modelItr;
            ref->InsertPartialData(modelData.vdataindex(), vertexData);
        }
        else
        {
            ModelReference* ref = new ModelReference(modelData.modelname());
            currentScene->modelsList.push_back(ref);
            ref->InsertPartialData(modelData.vdataindex(), vertexData); 
        }
    }

    grpc::Status status = reader->Finish();
    if (status.ok())
    {
        for (auto ref : currentScene->modelsList)
        {
            ref->LoadModelData();
        }

        std::cout << "SUCCESS: MODELS CREATED\n";
    }
    else
    {
        std::cout << "FAIL TO RECEVIVE MODEL " << status.error_code() << " " << status.error_message() << " " << status.error_details() << "\n";
    }
}

void SceneViewerClient::LoadTexturesInScene(int id)
{
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
        return;
    } 

    std::map<std::string, GLint> textureColorChannels; 

    IntValue sceneID; 
    sceneID.set_value(id); 

    grpc::ClientContext context;
    std::unique_ptr<grpc::ClientReader<TextureData>> reader(stub->LoadTexturesInScene(&context, sceneID));

    TextureData texData;
    while (reader->Read(&texData))
    {
        std::string name = texData.texturename();
        float width = texData.width();
        float height = texData.height();
        GLint imageFormat = texData.hasalpha() ? GL_RGBA : GL_RGB;
        unsigned bytesPerPixel = texData.hasalpha() ? 4 : 3; 
        int index = texData.pixelindex() * bytesPerPixel; 

        auto textureItr = std::find_if(currentScene->texturesList.begin(), currentScene->texturesList.end(), [&](Texture* x) { 
            return x->GetTextureName() == name; 
            });

        Texture* ref = NULL;
        if (textureItr != currentScene->texturesList.end())
        {
            ref = *textureItr;
        }
        else
        {
            ref = new Texture(name, width, height);
            currentScene->texturesList.push_back(ref); 
        }

        for (int i = 0; i < texData.pixeldatabatch_size(); i++)
        {
            PixelData pixel = texData.pixeldatabatch(i);
            ref->InsertPartialData(index + i * bytesPerPixel, bytesPerPixel, pixel.r(), pixel.g(), pixel.b(), pixel.a()); 
        }

        if (!textureColorChannels.contains(name))
        {
            textureColorChannels[name] = imageFormat;
        }
    }

    grpc::Status status = reader->Finish();
    if (status.ok())
    {
        for (auto ref : currentScene->texturesList)
        {
            ref->LoadTextureData(textureColorChannels[ref->GetTextureName()]);
        }

        std::cout << "SUCCESS: TEXTURES CREATED\n";
    }
    else
    {
        std::cout << "FAIL TO RECEIVE TEXTURE " << status.error_code() << " " << status.error_message() << " " << status.error_details() << "\n";
    } 
}

void SceneViewerClient::LoadObjectsInScene(int id)
{
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
        return;
    }


    IntValue sceneID;
    sceneID.set_value(id);

    grpc::ClientContext context;
    std::unique_ptr<grpc::ClientReader<ObjectData>> reader(stub->LoadObjectsInScene(&context, sceneID));

    ObjectData objData;
    while (reader->Read(&objData))
    {
        auto modelItr = std::find_if(currentScene->modelsList.begin(), currentScene->modelsList.end(), [&](ModelReference* x) {
            return x->GetModelName() == objData.modelname();
            });
        ModelReference* model = *modelItr;

        auto textureItr = std::find_if(currentScene->texturesList.begin(), currentScene->texturesList.end(), [&](Texture* x) {
            return x->GetTextureName() == objData.texturename();
            });
        Texture* texture = *textureItr;

        Model3D* obj = new Model3D(model, texture);
        obj->transform.position = glm::vec3(objData.position().x(), objData.position().y(), objData.position().z()); 
        obj->transform.Rotate(glm::vec3(objData.rotation().x(), objData.rotation().y(), objData.rotation().z())); 
        obj->transform.scale = glm::vec3(objData.scale().x(), objData.scale().y(), objData.scale().z()); 
        currentScene->objectsList.push_back(obj); 
    }

    grpc::Status status = reader->Finish();
    if (status.ok())
    {
        currentScene->isAlreadyLoaded = true;
        currentScene->loadingProgress = 100.f;
        std::cout << "SUCCESS: OBJECTS CREATED\n";
    }
    else
    {
        std::cout << "FAIL TO RECEIVE TEXTURE " << status.error_code() << " " << status.error_message() << " " << status.error_details() << "\n";
    }

}

void SceneViewerClient::GetSceneProgress(int id)
{

}