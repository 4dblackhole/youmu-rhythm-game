#pragma once
#include "framework.h"
#include "SingletonBase.h"

class FmodSystem : public SingletonBase<FmodSystem>
{
public:
	FmodSystem();
	~FmodSystem();

	FMOD_RESULT Init(FMOD_OUTPUTTYPE outputMode, unsigned int bufferlength);
	FMOD_RESULT Release();

	FMOD_RESULT ChangeDrive(int idx);

	inline int GetDriveIdx() const { return currentDriveIdx; }
	inline void SetDriveIdx(int v) { currentDriveIdx = v; }

	inline int GetDriveCount() const { return driveCount; }

	FMOD::System* System() { return _system; }

	vector<wstring> EnumDriverList();
	
	FMOD::Sound* GetSound(const string& str);
	FMOD::Channel*& GetChannel(const string& str);

	void Play(const string& str);
	void Stop(const string& str);

	class Name
	{
	public:
		DECLARE_VARIABLE_STRING(button01a);
		DECLARE_VARIABLE_STRING(select05);
	};

private:
	void InitSounds();
	map<string, pair<FMOD::Sound*,FMOD::Channel*>> defaultSoundList;

	FMOD::System* _system = nullptr;
	int driveCount = 0, currentDriveIdx = 0;

	unsigned int version = 0;
	void* extradriverdata = nullptr;
};

#define FMODSYSTEM FmodSystem::GetInstance()