#include "framework.h"
#include "ScorePercentage.h"

ScorePercentage::ScorePercentage(UINT64 pr)
	:precision(pr)
{
	Init();
}

ScorePercentage::~ScorePercentage()
{
}

void ScorePercentage::Init()
{
	denominator = 0;
	val = 0;
}

void ScorePercentage::AddScore(INT64 score)
{
	val += score;
	denominator += precision;
}

void ScorePercentage::AddScoreRate(double percentage)
{
	val += (INT64)((double)precision * percentage);
	denominator += precision;
}

INT64 ScorePercentage::GetScore(UINT64 maxScore) const
{
	if (denominator == 0) return 0;
	return INT64((double)maxScore * ((double)val / (double)denominator));
}
