#include "Texture.h"
#include "../stb_image.h"

Texture::Texture(std::string textureName, const char* imagePath) : textureName(textureName), imagePath(imagePath)
{
    isLoaded = false;
    width = 0;
    height = 0;
    colorChannels = GL_RGB;
    tex_bytes = NULL;
    pixels = NULL;
    texture = NULL;
}

Texture::Texture(std::string textureName, int width, int height) : textureName(textureName), width(width), height(height)
{
    isLoaded = false;
    imagePath = NULL; 
    colorChannels = GL_RGB; 
    tex_bytes = NULL; 
    pixels = NULL; 
    texture = NULL;
}

Texture::~Texture()
{
    if (pixels != NULL)
    {
        delete pixels;
    }
}

void Texture::LoadTexture(GLint imageFormat)
{
    if (imagePath == NULL)
    {
        return;
    }

    stbi_set_flip_vertically_on_load(true);
    tex_bytes = stbi_load(imagePath, &width, &height, &colorChannels, 0);
    SetupOpenGLTexture(imageFormat); 
}

void Texture::LoadTextureData(GLint imageFormat) 
{
    SetupOpenGLTexture(imageFormat); 
}

void Texture::InsertPartialData(int index, unsigned bytesPerPixel, unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
    if (pixels == NULL)
    {
        pixels = new GLubyte[width * height * bytesPerPixel];
    }

    pixels[index] = r;
    pixels[index+1] = g;
    pixels[index+2] = b;
    pixels[index+3] = a;
}

bool Texture::IsTextureLoaded()
{
    return isLoaded;
}

std::string Texture::GetTextureName()
{
    return textureName;
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

    glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, (tex_bytes != NULL ? tex_bytes : pixels));
    glGenerateMipmap(GL_TEXTURE_2D); 
    glEnable(GL_DEPTH_TEST); 

    if (tex_bytes != NULL) 
    {
        stbi_image_free(tex_bytes);
    }

    isLoaded = true;
}