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
	nominator = 0;
}

void ScorePercentage::AddScore(INT64 score)
{
	nominator += score;
	denominator += precision;
}

void ScorePercentage::AddScoreRate(double percentage)
{
	nominator += (INT64)((double)precision * percentage);
	denominator += precision;
}

double ScorePercentage::GetRate(double maxRate) const
{
	if (denominator == 0.0) return 0;
	return maxRate * (double)nominator / (double)denominator;
}

INT64 ScorePercentage::GetScore(UINT64 maxScore) const
{
	if (denominator == 0) return 0;
	return INT64((double)maxScore * ((double)nominator / (double)denominator));
}
