#pragma once
#include "libraries.h"

class MusicalPosition;
class MusicalNote;
class Measure;
class MusicBPM;

#include "PrefixSum/BpmTimingPrefixSum.h"
#include "PrefixSum/MeasurePrefixSum.h"

class MusicScore
{
public:
	MusicScore();
	~MusicScore();
	
	const MusicalNote* GetFirstNote() const;

	using NoteListPerType = map<MusicalPosition, MusicalNote>;

	//idx - length
	vector<Measure> measures;

	//time order
	set<MusicBPM> bpms;

	BpmTimingPrefixSum bpmPrefixSum;
	MeasurePrefixSum measurePrefixSum;
	void InitTimeSignaturePrefixSum();

	using NoteContainer = unordered_map<size_t, NoteListPerType>;
	NoteContainer notesPerTypeMap;

	double baseBpm = 120.0;
	chrono::microseconds offset{ 0 };

public:
	chrono::microseconds GetNoteTimingPoint(const MusicalPosition& note) const;
};

class Measure
{
public:
	constexpr Measure() {};
	constexpr Measure(const Measure& val) :length(val.length), visible(val.visible) {};
	constexpr Measure(const RationalNumber<64>& q, bool v = true) :length(q), visible(v) {}
	~Measure() {};

	RationalNumber<64> length;
	bool visible = true;
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

class MusicalNote : public MusicalObject
{
public:
	MusicalNote() {}
	~MusicalNote() {}

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
