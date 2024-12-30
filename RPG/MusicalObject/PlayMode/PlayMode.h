#pragma once
#include "libraries.h"
#include "MusicalObject/Lane.h"
#include "MusicalObject/AccuracyRange.h"
#include "MusicalObject/ScorePercentage.h"

class PlayMode
{
public:
	PlayMode();
	virtual ~PlayMode();

	virtual void InitTextures() = 0;
	void ReleaseTextures();

	virtual void InitLaneList() = 0;
	void ReleaseLaneList();

protected:
	vector<Lane*> laneList;
	TextureManager textureList;

	int Combo = 0;
	ScorePercentage scorePercent;
};