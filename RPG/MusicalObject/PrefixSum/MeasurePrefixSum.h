#pragma once
#include "framework.h"

class MusicalPosition;

class MeasurePrefixSum
{
public:
	constexpr MeasurePrefixSum() {};
	~MeasurePrefixSum() {};

	void InitMeasurePrefixSum(const vector<Measure>* measureList);
	void ReleaseMeasurePrefixSum();

	inline void SetMeasureList(const vector<Measure>* ptr) { measureList = ptr; }

	RationalNumber<64> GetMeasurePrefixSum(int start, int end) const;
	RationalNumber<64> GetMeasurePrefixSum(int idx) const;

private:
	vector<RationalNumber<64>> measurePrefixSum;
	const vector<Measure>* measureList = nullptr; //weak ptr
};