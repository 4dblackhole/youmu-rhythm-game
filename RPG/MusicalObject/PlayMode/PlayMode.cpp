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
	this->textureList.Clear();
}

void PlayMode::ReleaseLaneList()
{
	for (Lane* const& it : laneList) delete it;
	laneList.clear();
	
}
