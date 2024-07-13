#pragma once
#include "framework.h"

class Note;

class Measure
{
	Measure() {};
	~Measure() {};
public:

private:
	//RationalNumber length;
	vector<Note*> notes;
};

class Note
{
public:
	Note() {};
	~Note() {};

private:
	//RationalNumber position;
	int keyType;
	int actionType;
	FMOD::Channel* hitSound;
	string extraData;

	UINT64 measureIdx;

	double posMs;
};

