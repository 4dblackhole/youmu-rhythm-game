#pragma once
#include "framework.h"

struct MusicalPosition;
class Note;
class Measure;
class MusicBPM;

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

struct MusicalPosition
{
public:
	constexpr MusicalPosition(size_t idx = 0, RationalNumber<64>pos = 0) noexcept : measureIdx(idx), position(pos) {}
	bool operator<(const MusicalPosition& v) const;
	bool operator>(const MusicalPosition& v) const;
	bool operator<=(const MusicalPosition& v) const;
	bool operator>=(const MusicalPosition& v) const;
	bool operator==(const MusicalPosition& v) const;
	size_t measureIdx = 0;
	RationalNumber<64> position;
};

class MusicalObject
{
public:
	MusicalObject() : mp() {}
	MusicalObject(size_t idx, RationalNumber<64> pos) : mp({ idx, pos }) {}
	virtual ~MusicalObject() {};

	bool operator<(const MusicalObject&v) const;
	bool operator>(const MusicalObject&v) const;
	bool operator==(const MusicalObject& v) const;

	MusicalPosition mp;
};

class Note : public MusicalObject
{
public:
	Note() {}
	virtual ~Note() {}

	enum class DataOrder
	{
		Beat,
		Key,
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
	MusicBPM(MusicalObject obj = {}, double b = 120.0) :MusicalObject(obj), bpm(b) {}
	virtual ~MusicBPM() {}

	double BPM() const { return bpm; }
	void SetBPM(double b);

protected:
	double bpm;
};

class MusicalEffect : public MusicalObject
{
public:
	MusicalEffect() {}
	virtual ~MusicalEffect() {}

protected:
	string type;
	string val;
};
