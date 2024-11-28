#pragma once
#include "libraries.h"
#include "MusicalObject/NoteRelated/HitCondition/HitCondition.h"
#include "MusicalObject/MusicalObjects.h"

class Note
{
public:
	Note(const MusicalPosition& pos);
	virtual ~Note() {}

protected:
	const MusicalPosition mp;
	UINT noteType;
	UINT actionType;
	bool isPassed;
	HitCondition* condition;
};
