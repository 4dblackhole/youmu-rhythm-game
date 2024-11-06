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

FMOD_RESULT Music::CreateStream(const char* nameUtf8,FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* exinfo)
{
	const FMOD_RESULT& result = FmodSystem::GetInstance().System()->createStream(nameUtf8, mode, exinfo, &this->music);
	unsigned int tempMs;
	music->getLength(&tempMs, FMOD_TIMEUNIT_MS);
	musicLength = chrono::milliseconds(tempMs);

	return result;
}

void Music::UpdateMusicFileSize(const wstring_view& musicDir)
{
	//get audio file size
	std::ifstream fin(musicDir.data(), std::ios::binary);
	fin.seekg(0, std::ios_base::end);
	musicFileSize = (size_t)fin.tellg();
	fin.close();
}
