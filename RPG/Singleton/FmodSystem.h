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

private:
	FMOD::System* _system = nullptr;
	int driveCount = 0, currentDriveIdx = 0;

	unsigned int version = 0;
	void* extradriverdata = nullptr;
};