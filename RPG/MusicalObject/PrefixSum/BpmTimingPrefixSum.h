#pragma once
#include "framework.h"

struct BpmTimingPrefixSum
{
public:
	BpmTimingPrefixSum() {};
	~BpmTimingPrefixSum() {};

	using BpmPrefixSumContainer = map<MusicalPosition, MilliDouble>;
private:
	BpmPrefixSumContainer bpmTimingPrefixSum;

public:
	void InitBpmTimingPrefixSum(const set<MusicBPM>* list, const MeasurePrefixSum& measurePrefixSum);
	void ReleaseBpmTimingPrefixSum();

	inline void SetBpmList(const set<MusicBPM>* ptr) { bpmList = ptr; }

	const BpmPrefixSumContainer::const_iterator GetBpmTimingPoint(const BpmPrefixSumContainer::key_type& val) const;

private:
	const set<MusicBPM>* bpmList = nullptr; //weak ptr

};