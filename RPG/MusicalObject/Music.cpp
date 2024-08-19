#include "framework.h"
#include "Music.h"

Music::Music()
{
}

Music::~Music()
{
	music->release();
}

void Music::PlayMusic(bool pause)
{
	FMODSYSTEM.System()->playSound(music, nullptr, pause, &channel);
}

void Music::ChangeMusicPosition(unsigned int pos, FMOD_TIMEUNIT unit)
{
	channel->setPosition(pos, unit);
}
