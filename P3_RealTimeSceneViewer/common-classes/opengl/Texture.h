#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Texture
{
public:
	Texture(const char* imagePath);
	~Texture();

	void LoadTexture(GLint imageFormat);
	void LoadTextureData(int width, int height, GLint imageFormat, unsigned char* tex_bytes);

	GLuint& GetTexture();
	int GetWidth();
	int GetHeight();
	unsigned char* GetTextureBytes();

private:
	void SetupOpenGLTexture(GLint imageFormat);


private:
	int width;
	int height;
	int colorChannels;

	const char* imagePath;
	unsigned char* tex_bytes;
	std::string* tex_bytes2;
	GLuint texture;
};

