#include "framework.h"
#include "Music.h"

Music::Music()
{
}

Music::~Music()
{
	music->release();
}
