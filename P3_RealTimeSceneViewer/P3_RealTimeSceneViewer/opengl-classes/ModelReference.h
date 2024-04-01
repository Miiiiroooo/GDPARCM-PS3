#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../tiny_obj_loader.h"
#include <vector>
#include <string>

class ModelReference
{
public:
	ModelReference(std::string modelPath);
	~ModelReference();

    bool LoadObject();
    std::vector<GLfloat>& GetFullVertexData();
    GLuint GetVAO();
    void DeleteBufferObjects();

private:
    void SetupBufferObjects();


private:
    std::string modelPath;

    tinyobj::attrib_t attributes; 
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<GLuint> mesh_indices;
    std::vector<GLfloat> fullVertexData;

    GLuint VAO;
    GLuint VBO;
};

