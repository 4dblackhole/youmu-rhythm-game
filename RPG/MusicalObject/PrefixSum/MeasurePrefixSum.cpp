#include "framework.h"
#include "MeasurePrefixSum.h"

void MeasurePrefixSum::InitMeasurePrefixSum(const vector<Measure>* list)
{
	measureList = list;
	const vector<Measure>& currentMeasureList = *measureList;
	if (currentMeasureList.empty()) return;

	ReleaseMeasurePrefixSum();
	measurePrefixSum.reserve(currentMeasureList.size());

	vector<Measure>::const_iterator it = currentMeasureList.begin();
	measurePrefixSum.emplace_back((it++)->length);

	for (size_t prevIdx = 0; it != currentMeasureList.cend(); ++prevIdx, ++it)
	{
		const auto& val = measurePrefixSum[prevIdx] + it->length;
		measurePrefixSum.emplace_back(val);
	}
	DEBUG_BREAKPOINT;
}

void MeasurePrefixSum::ReleaseMeasurePrefixSum()
{
	measurePrefixSum.clear();
}

RationalNumber<64> MeasurePrefixSum::GetMeasurePrefixSum(int start, int end) const
{
	if (end < start) return 0;
	if (start == end)
	{
		const int& idx = std::clamp(start, 0, (int)measureList->size() - 1);
		return measureList->at(idx).length;
	}

	const RationalNumber<64>& endSum = GetMeasurePrefixSum(end);
	if (start < 1) return endSum;
	const RationalNumber<64>& prevSum = GetMeasurePrefixSum(start - 1);
	return endSum - prevSum;
}

RationalNumber<64> MeasurePrefixSum::GetMeasurePrefixSum(int idx) const
{
	if (idx < 0) return 0;
	const size_t& measureListSize = measurePrefixSum.size();
	if (idx < (int)measureListSize) return measurePrefixSum[idx];
	else
	{
		return measurePrefixSum.back() + measureList->back().length * (idx - (int)measureListSize + 1);
	}
}
