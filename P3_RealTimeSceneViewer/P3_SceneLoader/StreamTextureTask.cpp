#include "StreamTextureTask.h"
#include "opengl/Texture.h"

StreamTextureTask::StreamTextureTask(int sceneID, int max, std::string textureName, std::string texturePath, grpc::ServerWriter<TextureData>* writer)
	: sceneID(sceneID), max(max), textureName(textureName), texturePath(texturePath), writer(writer)
{
	switch (sceneID)
	{
		case 1:
			guard = &ServerSemaphore::streamTextureGuardSem1;
			finishedSem = &ServerSemaphore::finishedStreamTextureSem1;
			break;
		case 2:
			guard = &ServerSemaphore::streamTextureGuardSem2;
			finishedSem = &ServerSemaphore::finishedStreamTextureSem2;
			break;
		case 3:
			guard = &ServerSemaphore::streamTextureGuardSem3;
			finishedSem = &ServerSemaphore::finishedStreamTextureSem3;
			break;
		case 4:
			guard = &ServerSemaphore::streamTextureGuardSem4;
			finishedSem = &ServerSemaphore::finishedStreamTextureSem4;
			break;
		case 5:
			guard = &ServerSemaphore::streamTextureGuardSem5;
			finishedSem = &ServerSemaphore::finishedStreamTextureSem5;
			break;
		default:
			delete this;
			break;
	}
}

StreamTextureTask::~StreamTextureTask()
{

}

void StreamTextureTask::ExecuteTask()
{ 
	Texture* texture = new Texture(textureName, texturePath.c_str());   
	texture->LoadTexture(GL_RGBA);  
	unsigned char* tex_bytes = texture->GetTextureBytes(); 

	int width = texture->GetWidth(); 
	int height = texture->GetHeight(); 

	int index = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			unsigned bytePerPixel = 4;
			unsigned char* pixelOffset = tex_bytes + (j + width * i) * bytePerPixel;
			unsigned int r = (unsigned int)(pixelOffset[0] & 0xff);
			unsigned int g = (unsigned int)(pixelOffset[1] & 0xff);
			unsigned int b = (unsigned int)(pixelOffset[2] & 0xff);
			unsigned int a = (unsigned int)(pixelOffset[3] & 0xff);

			PixelData* pixelData = new PixelData();
			pixelData->set_r(r);
			pixelData->set_g(g);
			pixelData->set_b(b);
			pixelData->set_a(a);

			TextureData textureData = TextureData();
			textureData.set_texturename(textureName); 
			textureData.set_width(width);
			textureData.set_height(height);
			textureData.set_hasalpha(true);
			textureData.set_pixelindex(index);
			textureData.set_allocated_pixeldata(pixelData);
			writer->Write(textureData);

			index++;
		}
	}

	guard->acquire();
	if (!ServerSemaphore::finishedStreamTextureMap.contains(sceneID))
	{
		ServerSemaphore::finishedStreamTextureMap[sceneID] = 1;
	}
	else
	{
		ServerSemaphore::finishedStreamTextureMap[sceneID]++;
	}

	if (ServerSemaphore::finishedStreamTextureMap[sceneID] == max)
	{
		finishedSem->release();
	}
	guard->release();

	delete this;
}