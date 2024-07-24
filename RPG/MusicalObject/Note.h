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

private:
	vector<Measure*> measures;
	vector<MusicBPM*> bpms;
};

class Measure
{
	Measure() {};
	~Measure() {};
public:

private:
	RationalNumber<64> length;
	vector<Note*> notes;
};

class MusicalObject
{
public:
	MusicalObject() {};
	virtual ~MusicalObject() = 0;

protected:
	RationalNumber<64> position;

};

class Note : public MusicalObject
{
public:
	Note() {};
	virtual ~Note() {};

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
	~MusicBPM() {};

protected:
	double bpm;
};