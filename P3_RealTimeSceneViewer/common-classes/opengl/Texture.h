#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

class Texture
{
public:
	Texture(std::string textureName, const char* imagePath);
	Texture(std::string textureName, int width, int height);
	~Texture();

	void LoadTexture(GLint imageFormat);
	void LoadTextureData(GLint imageFormat);
	void InsertPartialData(int index, unsigned bytesPerPixel, unsigned int r, unsigned int g, unsigned int b, unsigned int a);
	bool IsTextureLoaded();

	std::string GetTextureName();
	GLuint& GetTexture();
	int GetWidth();
	int GetHeight();
	unsigned char* GetTextureBytes();

private:
	void SetupOpenGLTexture(GLint imageFormat);


private:
	std::string textureName;
	bool isLoaded;

	int width;
	int height;
	int colorChannels;

	const char* imagePath;
	unsigned char* tex_bytes;
	GLubyte* pixels;
	GLuint texture;
};