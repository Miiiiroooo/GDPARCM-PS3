#pragma once

#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<string>
#include<vector>
#include<iostream>

#include"tiny_obj_loader.h"




#include<unordered_map>

using namespace std;

class PerspectiveCameraObject;
class LightObject;

class ModelObject
{
public:
    ModelObject();
    void SetupModel(string path);
    void SetupTexture(string path, GLuint imageFormat);
    void Draw(GLuint ShaderProgram, PerspectiveCameraObject perspectiveCamera, LightObject light);
    void DeleteBuffers();

    void SetPosition(glm::vec3 pos);
    void SetScale(glm::vec3 scale);

  

private: 

    GLuint texture;

    std::string Path = "../3D/amumu.obj";
    std::vector<tinyobj::shape_t> Shapes;
    std::vector<tinyobj::material_t> Material;
    std::string warning, error;

    tinyobj::attrib_t Attributes;

    bool Success;

    std::vector<GLuint> indices;
    std::vector<GLfloat> fullVertexData;

    //Texutures
    int img_width, img_height, colorChannels;
    unsigned char* tex_bytes;

    //Buffers
    GLuint VAO, VBO;

    //Transform
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);
    glm::mat4 Transform;

    //Loaded Bools
    bool loadedModel = false;
    bool loadedTexture = false;
};

