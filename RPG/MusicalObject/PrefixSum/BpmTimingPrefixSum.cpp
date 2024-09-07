#include "framework.h"
#include "BpmTimingPrefixSum.h"

void BpmTimingPrefixSum::InitBpmTimingPrefixSum(const set<MusicBPM>* list, const MeasurePrefixSum& measurePrefixSum)
{
	ReleaseBpmTimingPrefixSum();
	bpmTimingPrefixSum.emplace(make_pair(MusicalPosition({ 0, RationalNumber<64>(0) }), 0));
	bpmList = list;
	if (bpmList->empty()) return;

	//initiation variables for loop
	set<MusicBPM>::const_iterator nextBpmSignature = bpmList->begin();
	MusicBPM currentBpmSignature(*nextBpmSignature); // 0-pos, basebpm
	++nextBpmSignature;

	while (nextBpmSignature != bpmList->cend())
	{
		const decltype(bpmTimingPrefixSum)::const_iterator& lastSumIter = std::prev(bpmTimingPrefixSum.end());
		const MusicalPosition& currentPos = currentBpmSignature.mp;
		const double& currentBPM = currentBpmSignature.BPM();

		const RationalNumber<64>& currentMeasureArea = measurePrefixSum.GetMeasurePrefixSum((int)currentPos.measureIdx, (int)nextBpmSignature->mp.measureIdx - 1);
		const RationalNumber<64>& currentBpmArea = currentMeasureArea - currentBpmSignature.mp.position + nextBpmSignature->mp.position;

		constexpr size_t msPerCommonTimeOfBPM60 = 240000;
		const double& msOfCurrentArea = (double)currentBpmArea * ((double)msPerCommonTimeOfBPM60 / currentBPM);
		const double& resultTiming = lastSumIter->second.count() + msOfCurrentArea;
		bpmTimingPrefixSum[nextBpmSignature->mp] = decltype(bpmTimingPrefixSum)::mapped_type(resultTiming);

		currentBpmSignature = *nextBpmSignature;
		++nextBpmSignature;
	}
}

void BpmTimingPrefixSum::ReleaseBpmTimingPrefixSum()
{
	bpmTimingPrefixSum.clear();
}

const BpmTimingPrefixSum::BpmPrefixSumContainer::const_iterator BpmTimingPrefixSum::GetBpmTimingPoint(const BpmPrefixSumContainer::key_type& val) const
{
	return bpmTimingPrefixSum.lower_bound(val);
}
