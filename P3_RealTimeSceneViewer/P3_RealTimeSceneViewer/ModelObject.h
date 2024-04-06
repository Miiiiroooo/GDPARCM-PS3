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

#include <map>


using namespace std;

class PerspectiveCameraObject;
class LightObject;

class ModelObject
{
public:
    ModelObject();
    ModelObject(string name);
    ModelObject(ModelObject* object);

    string GetModelName();

    void SetupModel(string path);
    void SetupTexture(string path, GLuint imageFormat);
    void Draw(GLuint ShaderProgram, PerspectiveCameraObject perspectiveCamera, LightObject light);
    void DeleteBuffers();

    void SetPosition(glm::vec3 pos);
    void SetScale(glm::vec3 scale);

    void InsertPartialData(int index, std::vector<float> partialData);
    void LoadModelData();
    void ProcessPartialVertexData();
    void InsertPartialTextureData(float width, float height, int index, unsigned bytesPerPixel, unsigned int r, unsigned int g, unsigned int b, unsigned int a);
    void LoadTextureData(GLint imageFormat);

    //Loaded Bools
    bool loadedModel = false;
    bool loadedTexture = false;

private: 
    string name;

    //textures
    GLubyte* pixels;
    GLuint texture;
    float width;
    float height;

    std::string Path = "";
    std::vector<tinyobj::shape_t> Shapes;
    std::vector<tinyobj::material_t> Material;
    std::string warning, error;

    tinyobj::attrib_t Attributes;

    bool Success;

    std::vector<GLuint> indices;
    std::vector<GLfloat> fullVertexData;
    std::map<int, std::vector<float>> partialVertexDataMap;

    //Texutures
    int img_width, img_height, colorChannels;
    unsigned char* tex_bytes;

    //Buffers
    GLuint VAO, VBO;

    //Transform
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);
    glm::mat4 Transform;
    glm::mat4 TranslationMatrix = glm::mat4(1.0f);
    glm::mat4 ScaleMatrix = glm::mat4(1.0f);
    glm::mat4 RotationMatrix = glm::mat4(1.0f);

    
};

