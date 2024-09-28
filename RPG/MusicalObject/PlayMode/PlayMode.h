#pragma once
#include "libraries.h"
#include "../Lane.h"
#include "../AccuracyRange.h"

class PlayMode
{
public:
	PlayMode();
	virtual ~PlayMode();

	virtual void InitTextures() = 0;
	void ReleaseTextures();

	vector<Lane*> LaneList;
	AccuracyRange AccRange;
	TextureManager TextureList;
};