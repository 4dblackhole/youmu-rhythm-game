#pragma once
#include "framework.h"

class AccuracyRange
{

public:
	enum class RangeName
	{
		Max, // 100% of score in the whole area
		Perfect, // 99% of score at the border between Great
		Great, // Gets 90% of score at the border 
		Good, // Gets 0% of score at the border 
		Bad, //not played incorrectly but inaccurate
		MAX
	};
public:

	struct ScoreInfo
	{
		double percentage{};
		XMFLOAT4 color = MyColor4::White;
	};

	struct Info
	{
		AccuracyRange::RangeName id = RangeName::MAX;
		MilliDouble range{};
		ScoreInfo scoreInfo;
		Info* prevInfo = nullptr; //for interpolation

		const ScoreInfo GetInterpolatedScoreInfo(const MilliDouble&, const chrono::microseconds&) const;

	};
	static constexpr AccuracyRange::Info NoAccInfo{ RangeName::MAX, MilliDouble(0.0), 0, {0,0,0,0} };

	//ex) MaxRange=4.5 -> +-4.5ms, total range is 9ms
	static constexpr size_t defaultAccLevel = 50;
	static constexpr AccuracyRange::Info DefaultAccInfo[(int)RangeName::MAX] =
	{
		{ RangeName::Max, MilliDouble(4.5),	1.0, MyColor4::White },
		{ RangeName::Perfect, MilliDouble(9.5),	0.99, {0.175f, 0.875f, 1, 1} },
		{ RangeName::Great,	MilliDouble(23.5), 0.9,	{0.15625f,1,0.15625f,1} },
		{ RangeName::Good, MilliDouble(54.5), 0, {1,0.75f,0.125f,1} },
		{ RangeName::Bad, MilliDouble(90.0), 0, {0.25f,0.09375f,1,1} }
	};

public:
	AccuracyRange(size_t level = defaultAccLevel);
	~AccuracyRange() {};

	void ChangeAccuracyLevel(size_t level);

	const AccuracyRange::Info& GetAccuracyInfo(RangeName n) const { return accRange[min((int)n, (int)RangeName::MAX - 1)]; }
	const MilliDouble& GetAccuracyRange(RangeName n) const { return accRange[min((int)n, (int)RangeName::MAX - 1)].range; }
	const size_t GetAccuracyLevel() const { return accLevel; }

	inline MilliDouble GetEarlyJudgeTiming(const MilliDouble refTime, const AccuracyRange::RangeName& judge) const
	{
		return refTime - GetAccuracyRange(judge);
	}
	inline MilliDouble GetLateJudgeTiming(const MilliDouble refTime, const AccuracyRange::RangeName& judge) const
	{
		return refTime + GetAccuracyRange(judge);
	}
	bool RangeCheck(const MilliDouble refTime, const chrono::microseconds& noteTime, const AccuracyRange::RangeName& judge) const;
	const AccuracyRange::Info* GetRangeInfo(const MilliDouble refTime, const chrono::microseconds& noteTime) const;

private:
	AccuracyRange::Info accRange[(int)RangeName::MAX];
	size_t accLevel;

	void InitAccuracyDefaultValue();

	const AccuracyRange::Info* GetRangeInfoBinarySearch(const MilliDouble refTime, const chrono::microseconds& noteTime,int left, int right) const;
};