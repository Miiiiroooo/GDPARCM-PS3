#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../tiny_obj_loader.h"

#include <vector>
#include <string>
#include <map>

class ModelReference
{
public:
	ModelReference(std::string modelName, std::string modelPath);
    ModelReference(std::string modelName);
	~ModelReference();

    bool LoadModel();
    void LoadModelData();
    void InsertPartialData(int index, std::vector<float> partialData);
    bool IsModelLoaded();

    std::string GetModelName();
    std::vector<GLfloat>& GetFullVertexData();
    GLuint GetVAO();
    void DeleteBufferObjects();

private:
    void SetupBufferObjects();
    void CleanupPartialDataMap();


private:
    std::string modelName;
    std::string modelPath;
    bool isLoaded;

    tinyobj::attrib_t attributes; 
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<GLuint> mesh_indices;

    std::vector<GLfloat> fullVertexData;
    std::map<int, std::vector<float>> partialVertexDataMap;

    GLuint VAO;
    GLuint VBO;
};

