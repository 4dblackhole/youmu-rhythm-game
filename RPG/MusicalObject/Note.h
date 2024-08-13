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

	vector<Measure*> measures;
	vector<MusicBPM*> bpms;
	vector<Note*> notes;

	double baseBpm = 120.0;
	double offset = 0.0;
};

class Measure
{
public:
	Measure() {};
	~Measure() {};

	RationalNumber<64> length;
};

class MusicalObject
{
public:
	MusicalObject() {}
	virtual ~MusicalObject() = 0;

	size_t GetMeasureIdx() const { return measureIdx; }

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
	string extraData;
};

class MusicBPM : public MusicalObject
{
public:
	MusicBPM(double b = 120.0) :bpm(b) {}
	virtual ~MusicBPM() {}

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