#include "MainMenuPanel.h"
#include "stb_image.h"
#include "SceneManager.h"

MainMenuPanel::MainMenuPanel(std::string name) : AUIPanel::AUIPanel(name) 
{
    GLuint scene1Thumbnail = loadTexture("../Sprites/scene1.png");
    sceneThumbnails.push_back(scene1Thumbnail);

    GLuint scene2Thumbnail = loadTexture("../Sprites/scene2.png");
    sceneThumbnails.push_back(scene2Thumbnail);

    GLuint scene3Thumbnail = loadTexture("../Sprites/scene3.png");
    sceneThumbnails.push_back(scene3Thumbnail);

    GLuint scene4Thumbnail = loadTexture("../Sprites/scene4.png");
    sceneThumbnails.push_back(scene4Thumbnail);

    GLuint scene5Thumbnail = loadTexture("../Sprites/scene5.png");
    sceneThumbnails.push_back(scene5Thumbnail);

    GLuint scene1SelectedThumbnail = loadTexture("../Sprites/scene1_selected.png");
    sceneSelectedThumbnails.push_back(scene1SelectedThumbnail);

    GLuint scene2SelectedThumbnail = loadTexture("../Sprites/scene2_selected.png");
    sceneSelectedThumbnails.push_back(scene2SelectedThumbnail);

    GLuint scene3SelectedThumbnail = loadTexture("../Sprites/scene3_selected.png");
    sceneSelectedThumbnails.push_back(scene3SelectedThumbnail);

    GLuint scene4SelectedThumbnail = loadTexture("../Sprites/scene4_selected.png");
    sceneSelectedThumbnails.push_back(scene4SelectedThumbnail);

    GLuint scene5SelectedThumbnail = loadTexture("../Sprites/scene5_selected.png");
    sceneSelectedThumbnails.push_back(scene5SelectedThumbnail);
}

void MainMenuPanel::draw()
{
    ImGui::SetNextWindowSize(ImVec2(800, 230));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Scene Bar", NULL, ImGuiWindowFlags_NoResize);

    for (int i = 0; i < 5; ++i) {
        ImGui::BeginGroup();

        //Cache Scene
        Scene* scene = SceneManager::getInstance()->GetSceneByID(i + 1);

        // Image
        if (scene->isActive)
        {
            ImGui::Image((void*)sceneSelectedThumbnails[i], ImVec2(150, 100));
        }
        else
        {
            ImGui::Image((void*)sceneThumbnails[i], ImVec2(150, 100));
        }
       

        // Progress bar
        int percentage = static_cast<int>(scene->GetLoadingProgress());
        std::string progressText = std::to_string(percentage) + "%";

        ImGui::ProgressBar(scene->GetLoadingProgress() / 100, ImVec2(150, 15), progressText.c_str());

        // Button
        if (ImGui::Button(("Select##" + std::to_string(i)).c_str(), ImVec2(150, 20))) {
            SceneManager::getInstance()->OpenSingleScene(i + 1);

            if (!scene->areResourcesStreamed)
            {
                savedScenes.clear();
                savedScenes.push_back(scene);
            }
            else 
            {
                savedScenes.clear();
            }
        }

        if (scene->areResourcesStreamed)
        {
            if (ImGui::Button(("X##" + std::to_string(i)).c_str(), ImVec2(150, 20))) {
                SceneManager::getInstance()->UnloadSingleScene(i + 1);
            }
        }
        else
        {
            if (scene->isLoading)
            {
                ImGui::Dummy(ImVec2(150, 20));
            }
            else
            {
                if (ImGui::Button(("Load Scene##" + std::to_string(i)).c_str(), ImVec2(150, 20))) {
                    SceneManager::getInstance()->LoadSingleScene(i + 1);
                }
            }
           
        }
        

        ImGui::EndGroup();

        // Add spacing between groups
        ImGui::SameLine();
    }

    // Add vertical spacing
    ImGui::Spacing();

    // Center align the "Load All" button
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 210) * 0.5f); // Adjust the button width as needed
    if (ImGui::Button("Open All", ImVec2(100, 20))) {
       
        if (!SceneManager::getInstance()->AllScenesLoaded())
        {
            savedScenes.clear();
            savedScenes = SceneManager::getInstance()->GetSceneList();
            SceneManager::getInstance()->CloseAllScenes();
        }
        else
        {
            savedScenes.clear();
            SceneManager::getInstance()->OpenAllScenes();
        }
    }

    ImGui::SameLine();

    // Center align the "Unload All" button
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10); // Adjust the spacing between buttons
    if (ImGui::Button("Unload All", ImVec2(100, 20))) {
        SceneManager::getInstance()->UnloadAllScenes();
    }

    ImGui::End();

    if (savedScenes.size() > 0)
    {
        ImGui::SetNextWindowSize(ImVec2(300, 50));
        ImGui::SetNextWindowPos(ImVec2(250, 350));
        ImGui::Begin("Loading...", NULL, ImGuiWindowFlags_NoResize);

        float progress = 0;

        for (int i = 0; i < savedScenes.size(); i++)
        {
            progress += savedScenes[i]->loadingProgress;
        }
        progress = progress / savedScenes.size();
        int percentage = static_cast<int>(progress);
        progress /= 100;
        std::string progressText = std::to_string(percentage) + "%";

        ImGui::ProgressBar(progress, ImVec2(280, 15), progressText.c_str());

        int counter = 0;
        for (int i = 0; i < savedScenes.size(); i++)
        {
            if (savedScenes[i]->areResourcesStreamed)
            {
                counter++;
            }
        }

        if (counter == savedScenes.size())
        {
            for (size_t i = 0; i < savedScenes.size(); i++)
            {
                savedScenes[i]->isActive = true;
            }
            savedScenes.clear();
        }

        ImGui::End();
    }
    
}

GLuint MainMenuPanel::loadTexture(const char* path)
{
    // Load image using stb_image
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (!data) {
        // Error handling
        return 0;
    }

    // Generate texture
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free image data
    stbi_image_free(data);

    return textureId;
}
