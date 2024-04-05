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

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraUp, cameraFront));
    glm::vec3 U = glm::cross(cameraFront, cameraRight);

    glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    //glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraFront, cameraUp);
};

