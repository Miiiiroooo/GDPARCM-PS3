#pragma once
#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<string>
#include<iostream>

using namespace std;

class LightObject
{
public:
    glm::vec3 GetLightPosition();
    glm::vec3 GetLightColor();
    void SetLightPosition(glm::vec3 Position);
    void SetLightColor(float r, float g, float b);
    void SetLightBrightness(float intensity);
    void SetPointLight(bool value);
    void SetDirectional(glm::vec3 directional);


    void ApplyLight(GLuint shader);

private:
    glm::vec3 lightPos = glm::vec3(2, 0, 0);
    glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 directional = glm::vec3(0.f, 0.f, 0.f);;

    float ambientStr = 0.9f;
    glm::vec3 ambientColor = lightColor;

    float specStr = 0.5f;
    float specPhong = 16;

    float brightness;
    float pointLight = true;
};

