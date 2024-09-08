#include "framework.h"
#include "VolumeSet.h"

void VolumeSet::VolumeNormalize()
{
	float maxVal = (float)max(musicVolume, hitsoundVolume);
	float volumeRateInv = 100.0f / maxVal;
	
	musicVolume = (int)((float)musicVolume * volumeRateInv);
	hitsoundVolume = (int)((float)hitsoundVolume * volumeRateInv);
}
