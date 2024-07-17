#pragma once
#include "framework.h"

class Music
{
public:
	Music();
	~Music();

	wstring MusicName;
	std::vector<std::wstring> MusicNameList;
	std::vector<std::wstring> ArtistList;
	std::set<std::wstring> TagList;

private:
	FMOD::Sound* music = nullptr;
	size_t musicFileSize = 0;
	UINT64 crc64 = 0;
};