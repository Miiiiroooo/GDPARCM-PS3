#pragma once

struct SceneLoadProgress 
{
	int progress = 0;
	int modelsMaxProgress = -1;
	int texturesMaxProgress = -1;
	int objectsMaxProgress = -1;
};