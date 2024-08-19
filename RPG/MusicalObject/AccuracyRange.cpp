#include "framework.h"
#include "AccuracyRange.h"

AccuracyRange::AccuracyRange(size_t level)
{
	ChangeAccuracyLevel(level);
}

void AccuracyRange::ChangeAccuracyLevel(size_t level)
{
	const float accRate = (float)defaultAccLevel / (float)level;
	for (int idx = 0; idx < (int)RangeName::MAX; ++idx)
	{
		accRange[idx] = DefaultAccRange[idx] * accRate;
	}
	accLevel = level;
}
