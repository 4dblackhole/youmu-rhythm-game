#pragma once
#include "framework.h"

class MusicalPosition;
class Note;
class Measure;
class MusicBPM;

#include "PrefixSum/BpmTimingPrefixSum.h"
#include "PrefixSum/MeasurePrefixSum.h"

class MusicScore
{
public:
	MusicScore();
	~MusicScore();
	
	const Note* GetFirstNote() const;

	using NoteList = map<MusicalPosition, Note>;

	//idx - length
	vector<Measure> measures;

	//time order
	set<MusicBPM> bpms;

	BpmTimingPrefixSum bpmPrefixSum;
	MeasurePrefixSum measurePrefixSum;
	void InitTimeSignaturePrefixSum();

	unordered_map<size_t, NoteList> notesPerTypeMap;

	double baseBpm = 120.0;
	chrono::microseconds offset{ 0 };

};

class Measure
{
public:
	Measure() {};
	Measure(const RationalNumber<64>& q) :length(q) {}
	Measure(RationalNumber<64>&& q) noexcept :length(q) {}
	~Measure() {};

	RationalNumber<64> length;
};

class MusicalPosition
{
public:
	constexpr MusicalPosition(const size_t& idx = 0, const RationalNumber<64>& pos = 0) : measureIdx(idx), position(pos) {}
	constexpr MusicalPosition(const MusicalPosition& m) : measureIdx(m.measureIdx), position(m.position) {}
	constexpr ~MusicalPosition() {}

	bool operator<(const MusicalPosition& v) const;
	bool operator>(const MusicalPosition& v) const;
	bool operator<=(const MusicalPosition& v) const;
	bool operator>=(const MusicalPosition& v) const;
	bool operator==(const MusicalPosition& v) const;

	size_t measureIdx;
	RationalNumber<64> position;

};

class MusicalObject
{
public:
	constexpr MusicalObject() : mp() {}
	constexpr MusicalObject(const MusicalPosition& val) : mp(val) {}
	constexpr MusicalObject(const size_t& idx, const RationalNumber<64>& pos) : mp({ idx, pos }) {}

	bool operator<(const MusicalObject& v) const;
	bool operator>(const MusicalObject& v) const;
	bool operator==(const MusicalObject& v) const;

	MusicalPosition mp;

};

class Note : public MusicalObject
{
public:
	Note() {}
	~Note() {}

	enum class DataOrder
	{
		Rhythm,
		Type,
		Action,
		Hitsound,
		ExtraData,
		MAX
	};

	UINT noteType = 0;
	UINT actionType = 0;
	wstring hitSound;
	wstring extraData;
};

class MusicBPM : public MusicalObject
{
public:
	constexpr MusicBPM(const MusicalObject& obj = {}, const double b = 120.0) :MusicalObject(obj), bpm(b) {}

	double BPM() const { return bpm; }
	void SetBPM(double b);

protected:
	double bpm;
};

class MusicalEffect : public MusicalObject
{
public:
	MusicalEffect() {}
	~MusicalEffect() {}

protected:
	string type;
	string val;
};
