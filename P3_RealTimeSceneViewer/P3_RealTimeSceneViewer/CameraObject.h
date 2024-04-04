#pragma once
#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<string>
#include<iostream>

using namespace std;

class CameraObject
{
public:
    CameraObject();
    glm::vec3 GetCameraPosition();

    glm::mat4 GetViewMatrix();
    glm::vec3 GetFront();
    void ApplyCameraPosition(GLuint ShaderProgram);
    void ApplyView(GLuint ShaderProgram);
    virtual void ApplyProjection(GLuint ShaderProgram) = 0;

protected:

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Center = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 F = glm::normalize(cameraPos - Center);

    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 R = glm::normalize(glm::cross(worldUp, F));
    glm::vec3 U = glm::cross(F, R);

    glm::mat4 viewMatrix = glm::lookAt(cameraPos, F, worldUp);
};

