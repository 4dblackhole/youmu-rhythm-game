#include "framework.h"
#include "AccuracyRange.h"

AccuracyRange::AccuracyRange(size_t level)
{
	ChangeAccuracyLevel(level);
}

void AccuracyRange::ChangeAccuracyLevel(size_t level)
{
	const double accRate = (double)defaultAccLevel / (double)level;
	for (int idx = 0; idx < (int)RangeName::MAX; ++idx)
	{
		accRange[idx] = DefaultAccRange[idx] * accRate;
	}
	accLevel = level;
}

bool AccuracyRange::RangeCheck(const MilliDouble refTime, const chrono::microseconds& noteTime, const AccuracyRange::RangeName& judge) const
{
	//out of range
	if (noteTime < GetEarlyJudgeTiming(refTime, judge) || noteTime > GetLateJudgeTiming(refTime, judge)) return false;
	else return true;
}
