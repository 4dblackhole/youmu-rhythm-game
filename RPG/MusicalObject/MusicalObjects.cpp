#include "framework.h"
#include "MusicalObjects.h"

MusicScore::MusicScore()
{
}

MusicScore::~MusicScore()
{
}

const MusicalNote* MusicScore::GetFirstNote() const
{
    if(notesPerTypeMap.empty()) return nullptr;
    const MusicalNote* result = nullptr;
    for (const decltype(notesPerTypeMap)::value_type& it : notesPerTypeMap)
    {
        const auto& firstNoteOfEachList = it.second.begin();
        if (firstNoteOfEachList == it.second.cend()) continue;
        if (result == nullptr)
        {
            result = &firstNoteOfEachList->second;
        }
        else
        {
            if (firstNoteOfEachList->first < result->mp) result = &firstNoteOfEachList->second;
        }
    }
    return result;
}

void MusicScore::InitTimeSignaturePrefixSum()
{
    measurePrefixSum.InitMeasurePrefixSum(&measures);
    bpmPrefixSum.InitBpmTimingPrefixSum(&bpms, measurePrefixSum);
}

chrono::microseconds MusicScore::GetNoteTimingPoint(const MusicalPosition& mp) const
{
    using namespace chrono;
    const BpmTimingPrefixSum::BpmPrefixSumContainer::const_iterator& bpmIter = this->bpmPrefixSum.GetBpmTimingPoint(mp);
    const MusicalPosition& bpmPos = bpmIter->first;
    const MilliDouble& bpmTiming = bpmIter->second;
    const double currentBpm = this->bpmPrefixSum.GetCurrentBPM(mp);

    //position from last bpm change
    const RationalNumber<64>& relativePos = this->measurePrefixSum.GetMeasurePrefixSum((int)bpmPos.measureIdx, (int)mp.measureIdx - 1) + mp.position - bpmPos.position;
    const MilliDouble& relativeTiming = MilliDouble((double)BPM1constantMeasure.count() * relativePos / currentBpm);

    const MilliDouble& resultTiming = duration_cast<MilliDouble>(this->offset) + bpmTiming + relativeTiming;
    auto result = microseconds((microseconds::rep)(duration_cast<MicroDouble>(resultTiming).count()));
    return microseconds((microseconds::rep)(duration_cast<MicroDouble>(resultTiming).count()));

}

const MusicalPosition MusicScore::AddMusicalPos(const MusicalPosition& notePos, RationalNumber<64> addLength) const
{
    const RationalNumber<64>& totalMeasureLength = measurePrefixSum.GetMeasurePrefixSum((int)notePos.measureIdx);
    const RationalNumber<64>& absolutePosition = totalMeasureLength + notePos.position;
    const RationalNumber<64>& resultPos = absolutePosition + addLength;

    const size_t& resultMeasureIdx = measurePrefixSum.GetMeasureIdxFromLength(resultPos);
    const RationalNumber<64>& newPos = resultPos - measurePrefixSum.GetMeasurePrefixSum((int)resultMeasureIdx);
    if (newPos < 0) return MusicalPosition(0, 0);
    else return MusicalPosition(resultMeasureIdx, newPos);
}

void MusicBPM::SetBPM(double b)
{
    bpm = b;
}

bool MusicalPosition::operator<(const MusicalPosition& v) const
{
	if (this->measureIdx < v.measureIdx) return true;
	if (this->measureIdx == v.measureIdx)
	{
		if (this->position < v.position) return true;
	}
	return false;
}

bool MusicalPosition::operator>(const MusicalPosition& v) const
{
	return v < *this;
}

bool MusicalPosition::operator<=(const MusicalPosition& v) const
{
	return !(*this > v);
}

bool MusicalPosition::operator>=(const MusicalPosition& v) const
{
	return !(*this < v);
}

bool MusicalPosition::operator==(const MusicalPosition& v) const
{
	return (this->measureIdx == v.measureIdx && this->position == v.position);
}

bool MusicalObject::operator<(const MusicalObject& v) const
{
    return this->mp < v.mp;
}

bool MusicalObject::operator>(const MusicalObject& v) const
{
    return this->mp > v.mp;
}

bool MusicalObject::operator==(const MusicalObject& v) const
{
    return this->mp == v.mp;
}