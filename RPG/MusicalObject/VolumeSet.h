#pragma once
#include "framework.h"

class VolumeSet
{
public:
	constexpr VolumeSet() : masterVolume(100), hitsoundVolume(100), musicVolume(100) {};
	constexpr ~VolumeSet() {};

	inline void SetMasterVolume(int val) { masterVolume = val; }
	inline void SetMusicVolume(int val) { musicVolume = val; }
	inline void SetHitSoundVolume(int val) { hitsoundVolume = val; }

	const int& MasterVolume() const { return masterVolume; }
	const int& MusicVolume() const { return musicVolume; }
	const int& HitSoundVolume() const { return hitsoundVolume; }

private:
	void VolumeNormalize();

	int masterVolume;
	int musicVolume;
	int hitsoundVolume;
};