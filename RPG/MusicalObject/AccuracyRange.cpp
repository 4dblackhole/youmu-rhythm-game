#include "framework.h"
#include "AccuracyRange.h"

AccuracyRange::AccuracyRange(size_t level)
{
	InitAccuracyDefaultValue();
	ChangeAccuracyLevel(level);
}

void AccuracyRange::ChangeAccuracyLevel(size_t level)
{
	const double accRate = (double)defaultAccLevel / (double)level;
	for (int idx = 0; idx < (int)RangeName::MAX; ++idx)
	{
		accRange[idx].range = DefaultAccInfo[idx].range * accRate;
	}
	accLevel = level;
}

bool AccuracyRange::RangeCheck(const MilliDouble refTime, const chrono::microseconds& noteTime, const AccuracyRange::RangeName& judge) const
{
	//out of range
	if (noteTime < GetEarlyJudgeTiming(refTime, judge) || noteTime > GetLateJudgeTiming(refTime, judge)) return false;
	else return true;
}

const AccuracyRange::Info* AccuracyRange::GetRangeInfo(const MilliDouble refTime, const chrono::microseconds& noteTime) const
{
	if (RangeCheck(refTime, noteTime, RangeName::Bad) == false) return nullptr;
	return GetRangeInfoBinarySearch(refTime, noteTime, 0, (int)RangeName::MAX - 1);
}

const AccuracyRange::Info* AccuracyRange::GetRangeInfoBinarySearch(const MilliDouble refTime, const chrono::microseconds& noteTime, int left, int right) const
{
	while (left < right)
	{
		const int mid = (left + right) / 2;

		if (RangeCheck(refTime, noteTime, (RangeName)mid) == false) left = mid + 1;
		else right = mid;
	}
	return &accRange[left];
}

void AccuracyRange::InitAccuracyDefaultValue()
{
	for (int idx = 0; idx < (int)RangeName::MAX; ++idx)
	{
		accRange[idx].id = DefaultAccInfo[idx].id;
		accRange[idx].scoreInfo = DefaultAccInfo[idx].scoreInfo;
		if (idx > 0) accRange[idx].prevInfo = &accRange[idx - 1];
	}
	accRange[(int)RangeName::Bad].prevInfo = nullptr;
}

const AccuracyRange::ScoreInfo AccuracyRange::Info::GetInterpolatedScoreInfo(const MilliDouble& refTime, const chrono::microseconds& noteTime) const
{
	if (prevInfo == nullptr) return scoreInfo;

	const MilliDouble& diff = abs(refTime - chrono::duration_cast<MilliDouble>(MicroDouble(noteTime.count())));
	const double interp = Interpolation_Linear(prevInfo->range.count(), range.count(), 0.0, 1.0, diff.count());
	const double interpInv = 1.0 - interp;

	const AccuracyRange::ScoreInfo& prevScoreInfo = prevInfo->scoreInfo;
	return
	{
		prevScoreInfo.percentage * interpInv + scoreInfo.percentage * interp,
		XMFLOAT4
		(
			prevScoreInfo.color.x * (float)interpInv + scoreInfo.color.x * (float)interp,
			prevScoreInfo.color.y * (float)interpInv + scoreInfo.color.y * (float)interp,
			prevScoreInfo.color.z * (float)interpInv + scoreInfo.color.z * (float)interp,
			prevScoreInfo.color.w * (float)interpInv + scoreInfo.color.w * (float)interp
		)
	};
}
