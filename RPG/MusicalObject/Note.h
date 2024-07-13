#pragma once
#include "framework.h"

class Note;

class Measure
{
	Measure() {};
	~Measure() {};
public:

private:
	RationalNumber<64> length;
	vector<Note*> notes;
};

class Note
{
public:
	Note() {};
	~Note() {};

private:
	RationalNumber<64> position;
	UINT keyType;
	UINT actionType;
	FMOD::Channel* hitSound;
	string extraData;
};

