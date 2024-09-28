#include "framework.h"
#include "PlayMode.h"

PlayMode::PlayMode()
{
}

PlayMode::~PlayMode()
{
	ReleaseTextures();
}

void PlayMode::ReleaseTextures()
{
	this->TextureList.Clear();
}
