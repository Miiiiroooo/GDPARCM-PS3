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

        auto modelItr = std::find_if(currentScene->modelRef.begin(), currentScene->modelRef.end(), [&](ModelObject* x) {
            return x->GetModelName() == modelData.modelname();
            });

        if (modelItr != currentScene->modelRef.end())
        {
            ModelObject* ref = *modelItr;
            ref->InsertPartialData(modelData.vdataindex(), vertexData);
        }
        else
        {
            ModelObject* ref = new ModelObject(modelData.modelname());
            currentScene->modelRef.push_back(ref);
            ref->InsertPartialData(modelData.vdataindex(), vertexData);
        }

        if (currentScene->loadingProgress < 0.40f)
        {
            currentScene->loadingProgress += 0.000004f;
        }
     
    }

    grpc::Status status = reader->Finish();
    if (status.ok())
    {
        currentScene->loadingProgress = 0.40f;
        /*for (auto ref : currentScene->modelsList)
        {
            ref->LoadModelData();
        }*/

        std::cout << "SUCCESS: MODELS CREATED ON SCENE" << std::to_string(id) << "\n";
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
        unsigned bytePerPixel = texData.hasalpha() ? 4 : 3;
        int index = texData.pixelindex() * bytePerPixel;

        auto textureItr = std::find_if(currentScene->modelRef.begin(), currentScene->modelRef.end(), [&](ModelObject* x) {
            return x->GetModelName() == name;
            });

        ModelObject* ref = NULL;
        if (textureItr != currentScene->modelRef.end())
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
            ref->InsertPartialTextureData(width, height, index + i * bytePerPixel, bytePerPixel, pixel.r(), pixel.g(), pixel.b(), pixel.a());
        }

        if (!textureColorChannels.contains(name))
        {
            textureColorChannels[name] = imageFormat;
        }

        if (currentScene->loadingProgress < 0.90f)
        {
            currentScene->loadingProgress += 0.000004f;
        }
    }

    grpc::Status status = reader->Finish();
    if (status.ok())
    {
        currentScene->loadingProgress = 0.90f;
        /*for (auto ref : currentScene->modelsList)
        {
            ref->LoadTextureData(textureColorChannels[ref->GetModelName()]);
        }*/

        std::cout << "SUCCESS: TEXTURES CREATED ON SCENE" << std::to_string(id) << "\n";
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
        auto modelItr = std::find_if(currentScene->modelRef.begin(), currentScene->modelRef.end(), [&](ModelObject* x) {
            return x->GetModelName() == objData.modelname();
            });

        ModelObject* model = *modelItr; //Ref

        ModelObject* modelGameObject = new ModelObject(model);

        modelGameObject->SetPosition(glm::vec3(objData.position().x(), objData.position().y(), objData.position().z()));
        modelGameObject->SetScale(glm::vec3(objData.scale().x(), objData.scale().y(), objData.scale().z()));

        modelGameObject->ProcessPartialVertexData();
        /*modelGameObject->LoadModelData();
        modelGameObject->LoadTextureData(GL_RGBA);*/

        currentScene->modelList.push_back(modelGameObject);
    }

    grpc::Status status = reader->Finish();
    if (status.ok())
    {
        currentScene->isAlreadyLoaded = true;
        currentScene->loadingProgress = 1.f;
        std::cout << "SUCCESS: OBJECTS CREATED ON SCENE" << std::to_string(id) << "\n";
    }
    else
    {
        std::cout << "FAIL TO RECEIVE TEXTURE " << status.error_code() << " " << status.error_message() << " " << status.error_details() << "\n";
    }

}

void SceneViewerClient::GetSceneProgress(int id)
{

}