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

	FMOD_RESULT CreateStream(const char* nameUtf8, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* exinfo);
	void UpdateMusicFileSize(const wstring_view& musicDir);

	const chrono::milliseconds GetMusicLength() const { return musicLength; }
	const size_t GetMusicFileSize() const { return musicFileSize; }
	const UINT64 GetCRC64() const { return crc64; }

	wstring ymmFileName;
	wstring FileName;
	std::vector<std::wstring> MusicNameList;
	std::vector<std::wstring> ArtistList;
	std::set<std::wstring> TagList;

	FMOD::Channel* channel = nullptr;

	vector<Pattern*> patternList;

private:
	FMOD::Sound* music = nullptr;
	chrono::milliseconds musicLength{};
	size_t musicFileSize = 0;
	UINT64 crc64 = 0;
};