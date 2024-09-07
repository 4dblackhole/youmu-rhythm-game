#include "framework.h"
#include "Note.h"

MusicScore::MusicScore()
{
}

MusicScore::~MusicScore()
{
}

const Note* MusicScore::GetFirstNote() const
{
    if(notesPerTypeMap.empty()) return nullptr;
    const Note* result = nullptr;
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