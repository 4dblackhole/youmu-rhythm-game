#pragma once
#include "Utility/RationalNumber.h"

class ScorePercentage
{
public:
	ScorePercentage(UINT64 pr = 100);
	~ScorePercentage();
	
	void Init();
	void AddScore(INT64);
	void AddScoreRate(double);

	//TODO: add weight

	double GetRate(double maxRate = 100.0) const;
	INT64 GetScore(UINT64 maxScore = 1000000) const;

	const UINT64 GetPrecision() const { return precision; }

private:
	const UINT64 precision;
	INT64 nominator;
	UINT64 denominator;
};