#include "Shader.h"
#include <fstream>
#include <sstream>

Shader::Shader()
{

}

Shader::~Shader()
{

}

void Shader::InitializeProgram(std::string vShaderPath, std::string fShaderPath)
{
    std::string vString = GetShaderData(vShaderPath);
    vShaderData = vString.c_str();    
    std::string fString = GetShaderData(fShaderPath);
    fShaderData = fString.c_str();

    vertShader = glCreateShader(GL_VERTEX_SHADER);   
    glShaderSource(vertShader, 1, &vShaderData, NULL);  
    glCompileShader(vertShader);  

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);   
    glShaderSource(fragShader, 1, &fShaderData, NULL);  
    glCompileShader(fragShader);  

    shaderProgram = glCreateProgram(); 
    glAttachShader(shaderProgram, vertShader);  
    glAttachShader(shaderProgram, fragShader);  
    glLinkProgram(shaderProgram); 
}

GLuint& Shader::GetShaderProgram()
{
    return shaderProgram;
}

std::string Shader::GetShaderData(std::string path)
{
    std::fstream src(path); 
    std::stringstream buffer; 
    buffer << src.rdbuf(); 

    return buffer.str();
}
