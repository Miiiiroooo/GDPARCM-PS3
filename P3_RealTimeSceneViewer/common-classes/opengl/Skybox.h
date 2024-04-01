#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

class Skybox
{
public:
	Skybox(GLuint* shaderProgram);
	~Skybox();

	void IntializeSkybox(const std::vector<std::string> skyboxFaces);
	void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, bool isNightVision);

private:
	void InitializeBufferObjects();
	void SetupTextures();


private:
	GLuint* skyboxShaderProgram;

	float* skyboxVerticestemp;
	unsigned int* skyboxIndicestemp;

	std::vector<float> skyboxVertices; 
	std::vector<unsigned int> skyboxIndices; 
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int skyboxEBO;

	std::vector<std::string> skyboxTextureFaces; 
	unsigned int skyboxTexture;
};

