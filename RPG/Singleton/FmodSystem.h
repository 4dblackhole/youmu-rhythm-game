#pragma once
#include "framework.h"
#include "SingletonBase.h"

class FmodSystem : public SingletonBase<FmodSystem>
{
public:
	FmodSystem();
	~FmodSystem();

	FMOD_RESULT Init(FMOD_OUTPUTTYPE outputMode, unsigned int bufferlength = 256);
	FMOD_RESULT Release();

	FMOD_RESULT ChangeDrive(int idx);
	FMOD_RESULT ChangeDriveOutputType(FMOD_OUTPUTTYPE type);

	inline int GetDriveIdx() const { return currentDriveIdx; }
	inline void SetDriveIdx(int v) { currentDriveIdx = v; }

	inline int GetDriveCount() const { return driveCount; }

	FMOD::System* System() { return mSystem; }

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

	FMOD::System* mSystem = nullptr;
	int driveCount = 0, currentDriveIdx = 0;

	unsigned int version = 0;
	void* extradriverdata = nullptr;
};

#define FMODSYSTEM FmodSystem::GetInstance()