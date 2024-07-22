#pragma once
#include "framework.h"

class Music
{
public:
	Music();
	~Music();

	wstring FileName;
	std::vector<std::wstring> MusicNameList;
	std::vector<std::wstring> ArtistList;
	std::set<std::wstring> TagList;
	FMOD::Sound* music = nullptr;
	FMOD::Channel* channel = nullptr;
	size_t musicFileSize = 0;
	UINT64 crc64 = 0;
};