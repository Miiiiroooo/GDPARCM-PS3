#include "StreamTextureTask.h"
#include "opengl/Texture.h"

StreamTextureTask::StreamTextureTask(int sceneID, int maxBatchSize, int maxThreads, std::string textureName, std::string texturePath, grpc::ServerWriter<TextureData>* writer)
	: sceneID(sceneID), maxBatchSize(maxBatchSize), maxThreads(maxThreads), textureName(textureName), texturePath(texturePath), writer(writer)
{
	/*switch (sceneID)
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
	}*/
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
	unsigned bytesPerPixel = 4;

	int index = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j += maxBatchSize)
		{
			unsigned char* pixelOffset = tex_bytes + (j + width * i) * bytesPerPixel;

			TextureData textureData = TextureData();
			textureData.set_texturename(textureName);
			textureData.set_width(width);
			textureData.set_height(height);
			textureData.set_hasalpha(true);
			textureData.set_pixelindex(index);

			for (int k = 0; k < maxBatchSize * bytesPerPixel; k += bytesPerPixel)
			{
				if (j + textureData.pixeldatabatch_size() == height)
				{
					break;
				}

				unsigned int r = (unsigned int)(pixelOffset[k] & 0xff);
				unsigned int g = (unsigned int)(pixelOffset[k + 1] & 0xff);
				unsigned int b = (unsigned int)(pixelOffset[k + 2] & 0xff);
				unsigned int a = (unsigned int)(pixelOffset[k + 3] & 0xff);

				PixelData* pixelData = textureData.add_pixeldatabatch();
				pixelData->set_r(r);
				pixelData->set_g(g);
				pixelData->set_b(b);
				pixelData->set_a(a);
			}

			writer->Write(textureData);

			index += maxBatchSize;
		}
	}

	/*guard->acquire();
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
	guard->release();*/

	delete this;
}