#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture
{
public:
	Texture(const char* imagePath);
	~Texture();

	void LoadTexture(GLint imageFormat);
	GLuint& GetTexture();


private:
	int width;
	int height;
	int colorChannels;

	const char* imagePath;
	unsigned char* tex_bytes;
	GLuint texture;
};

