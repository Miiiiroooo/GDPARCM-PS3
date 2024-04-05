#include "MainMenuPanel.h"
#include "stb_image.h"

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
}

void MainMenuPanel::draw()
{
    ImGui::SetNextWindowSize(ImVec2(800, 210));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Scene Bar", NULL, ImGuiWindowFlags_NoResize);

    for (int i = 0; i < 5; ++i) {
        ImGui::BeginGroup();

        // Image
        ImGui::Image((void*)sceneThumbnails[i], ImVec2(150, 100)); // Replace your_image_texture_id with your actual image texture ID

        // Progress bar
        float progress = 0.1f; // Replace with your actual loading progress
        int percentage = static_cast<int>(progress * 100);
        std::string progressText = std::to_string(percentage) + "%";

        ImGui::ProgressBar(progress, ImVec2(150, 15), progressText.c_str());

        // Button
        if (ImGui::Button("Load", ImVec2(150, 20))) {
            // Add logic to start loading process for this image
        }

        ImGui::EndGroup();

        // Add spacing between groups
        ImGui::SameLine();
    }

    // Add vertical spacing
    ImGui::Spacing();

    // Center align the "Load All" button
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 100) * 0.5f); // Adjust the button width as needed
    if (ImGui::Button("Load All", ImVec2(100, 20))) {
        // Add logic to start loading process for all images
    }

    ImGui::End();
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
