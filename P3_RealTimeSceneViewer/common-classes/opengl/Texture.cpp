#include "Texture.h"
#include "../stb_image.h"

Texture::Texture(const char* imagePath) : imagePath (imagePath)
{
    width = 0;
    height = 0;
    colorChannels = GL_RGB;
    tex_bytes = NULL;
    texture = NULL;
}

Texture::~Texture()
{

}

void Texture::LoadTexture(GLint imageFormat)
{
    stbi_set_flip_vertically_on_load(true);
    tex_bytes = stbi_load(imagePath, &width, &height, &colorChannels, 0);
    SetupOpenGLTexture(imageFormat); 
}

void Texture::LoadTextureData(int width, int height, GLint imageFormat, unsigned char* tex_bytes)
{
    this->width = width;
    this->height = height;
    this->tex_bytes = tex_bytes;
    SetupOpenGLTexture(imageFormat);
}

GLuint& Texture::GetTexture()
{
	return texture;
}

int Texture::GetWidth()
{
    return width;
}

int Texture::GetHeight()
{
    return height;
}

unsigned char* Texture::GetTextureBytes()
{
    return tex_bytes;
}

void Texture::SetupOpenGLTexture(GLint imageFormat)
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (window == NULL)
    {
        return;
    }

    glGenTextures(1, &texture); 
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, texture); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

    glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D); 
    stbi_image_free(tex_bytes);
    glEnable(GL_DEPTH_TEST); 
}