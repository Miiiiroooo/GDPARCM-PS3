#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture
{
public:
	Texture(const char* imagePath);
	~Texture();

	void LoadTexture(GLint imageFormat);
	void LoadTextureData(int width, int height, GLint imageFormat, GLubyte* pixels);

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
	GLubyte* pixels;
	GLuint texture;
};