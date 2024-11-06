#pragma once
#include "libraries.h"
#include "MusicalObject/NoteRelated/HitCondition/HitCondition.h"
#include "MusicalObject/MusicalObjects.h"

class Note
{
public:
	Note() {};
	~Note() {};

protected:
	MusicalPosition mp;
	UINT noteType;
	UINT actionType;
	bool isPassed;
	HitCondition* condition;
};
