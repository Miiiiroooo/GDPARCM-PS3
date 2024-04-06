#include "ModelReference.h"
#include <iostream>

ModelReference::ModelReference(std::string modelName, std::string modelPath) : modelName(modelName), modelPath(modelPath)
{
    isLoaded = false;
}

ModelReference::ModelReference(std::string modelName) : modelName(modelName), modelPath("")
{
    isLoaded = false;
}

ModelReference::~ModelReference()
{

}

bool ModelReference::LoadModel()
{
    if (modelPath == "")
    {
        std::cout << "model path not specified\n";
        return false;
    }

    std::string warning, error;

    if (tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, modelPath.c_str())) 
    {
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++) 
        {
            tinyobj::index_t vData = shapes[0].mesh.indices[i]; 

            mesh_indices.push_back(vData.vertex_index); 

            fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 0]); 
            fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 1]); 
            fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 2]); 

            fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 0]); 
            fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 1]); 
            fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 2]); 

            fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2 + 0]); 
            fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2 + 1]); 
        }
        
        SetupBufferObjects();
        CleanupPartialDataMap();

        return true; 
    }
    else
    {
        std::cout << "error loading obj file\n";
        std::cout << "Error: " << error << "\n";
        std::cout << "Warning: " << warning << "\n";
        return false;
    }
}

void ModelReference::LoadModelData()
{
    for (auto pair : partialVertexDataMap)
    {
        fullVertexData.insert(fullVertexData.end(), pair.second.begin(), pair.second.end());
    }

    SetupBufferObjects();
    CleanupPartialDataMap();
}

void ModelReference::InsertPartialData(int index, std::vector<float> partialData)
{
    partialVertexDataMap[index] = partialData;
}

bool ModelReference::IsModelLoaded()
{
    return isLoaded;
}

std::string ModelReference::GetModelName()
{
    return modelName;
}

std::vector<GLfloat>& ModelReference::GetFullVertexData()
{
    return fullVertexData;
}

GLuint ModelReference::GetVAO()
{
    return VAO;
}

void ModelReference::DeleteBufferObjects()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ModelReference::SetupBufferObjects()
{
    GLFWwindow* window = glfwGetCurrentContext(); 
    if (window == NULL) 
    { 
        return;
    }

    // Generate references
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &VBO); 

    // Bind the buffer objects
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * fullVertexData.size(), fullVertexData.data(), GL_STATIC_DRAW);

    // Reference the attrib pointers
    GLintptr normalPtr = 3 * sizeof(float);
    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)normalPtr);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)uvPtr);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    // Unbind the buffers objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Update status
    isLoaded = true;
}

void ModelReference::CleanupPartialDataMap()
{
    partialVertexDataMap.clear();
}