#pragma once
#include "framework.h"

class Note;
class Measure;
class MusicBPM;

class MusicScore
{
public:
	MusicScore() {};
	~MusicScore() {};
	 
	//idx - length
	vector<Measure> measures;

	//time order
	set<MusicBPM> bpms;
	map<size_t, set<Note>> notes;

	double baseBpm = 120.0;
	double offset = 0.0;
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

class MusicalObject
{
public:
	MusicalObject() {}
	MusicalObject(size_t idx, RationalNumber<64> pos) : measureIdx(idx), position(pos) {}
	virtual ~MusicalObject() {};

	size_t GetMeasureIdx() const { return measureIdx; }

	bool operator<(const MusicalObject&v) const;
	bool operator==(const MusicalObject& v) const;

protected:
	size_t measureIdx = 0;
	RationalNumber<64> position;

};

class Note : public MusicalObject
{
public:
	Note() {}
	virtual ~Note() {}

private:
	UINT keyType = 0;
	UINT actionType = 0;
	FMOD::Channel* hitSound = nullptr;
	wstring extraData;
};

class MusicBPM : public MusicalObject
{
public:
	MusicBPM(MusicalObject obj = {}, double b = 120.0) :MusicalObject(obj), bpm(b) {}
	virtual ~MusicBPM() {}

	double BPM() const { return bpm; }

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