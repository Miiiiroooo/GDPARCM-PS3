#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>


class Shader
{
public:
	Shader();
	~Shader();

	void InitializeProgram(std::string vShaderPath, std::string fShaderPath);
	GLuint& GetShaderProgram();

private:
	std::string GetShaderData(std::string path);


private:
	const char* vShaderData;
	const char* fShaderData;

	GLuint vertShader;
	GLuint fragShader;
	GLuint shaderProgram;
};

