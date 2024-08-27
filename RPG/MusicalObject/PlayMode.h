#pragma once
#include "framework.h"
#include "Lane.h"
#include "AccuracyRange.h"

class PlayMode
{
public:
	PlayMode();
	~PlayMode();

private:
	vector<Lane*> laneList;
	AccuracyRange accRange;
};