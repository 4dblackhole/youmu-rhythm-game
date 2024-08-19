#pragma once
#include "framework.h"

class AccuracyRange
{
public:
	enum class RangeName
	{
		Max,
		Perfect,
		Great,
		Good,
		Bad,
		Miss,
		MAX
	};

private:
	static constexpr size_t defaultAccLevel = 50;

public:
	//ex) MaxRange=4.5 -> +-4.5ms, total range is 9ms
	static constexpr float DefaultAccRange[(int)RangeName::MAX] =
	{ 4.5f, 12.5f, 23.5f, 37.5f, 50.0f, 90.0f };

public:
	AccuracyRange(size_t level = defaultAccLevel);
	~AccuracyRange() {};

	void ChangeAccuracyLevel(size_t level);

	const float* const GetAccuracyRange() const { return accRange; }
	const size_t GetAccuracyLevel() const { return accLevel; }

private:
	float accRange[(int)RangeName::MAX];
	size_t accLevel;
};