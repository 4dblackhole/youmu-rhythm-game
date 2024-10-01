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


	//ex) MaxRange=4.5 -> +-4.5ms, total range is 9ms
	static constexpr size_t defaultAccLevel = 50;
	static constexpr MilliDouble DefaultAccRange[(int)RangeName::MAX] =
	{ MilliDouble(4.5), MilliDouble(9.5), MilliDouble(23.5), MilliDouble(54.5), MilliDouble(90.0) };

public:
	AccuracyRange(size_t level = defaultAccLevel);
	~AccuracyRange() {};

	void ChangeAccuracyLevel(size_t level);

	const MilliDouble* GetAccuracyRange() const { return accRange; }
	const MilliDouble& GetAccuracyRange(RangeName n) const { return accRange[min((int)n, (int)RangeName::MAX - 1)]; }
	const size_t GetAccuracyLevel() const { return accLevel; }

private:
	MilliDouble accRange[(int)RangeName::MAX];
	size_t accLevel;
};