#pragma once
#include "framework.h"
#include "Pattern.h"

class Music
{
public:
	Music();
	~Music();

	void PlayMusic(bool pause = false);
	void ChangeMusicPosition(unsigned int pos, FMOD_TIMEUNIT unit);

	wstring ymmFileName;
	wstring FileName;
	std::vector<std::wstring> MusicNameList;
	std::vector<std::wstring> ArtistList;
	std::set<std::wstring> TagList;

	FMOD::Sound* music = nullptr;
	FMOD::Channel* channel = nullptr;
	size_t musicFileSize = 0;
	UINT64 crc64 = 0;

	vector<Pattern*> patternList;
};