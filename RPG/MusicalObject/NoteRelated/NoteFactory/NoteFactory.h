#pragma once
#include "libraries.h"
#include "MusicalObject/NoteRelated/Note/NoteObject.h"

class NoteFactory
{
public:
	NoteFactory() {};
	~NoteFactory() {};

	//dynamic alloc
	static NoteObject* CreateTaikoNote(UINT noteType, UINT actionType, const MusicalNote* p = nullptr, const chrono::microseconds t = chrono::microseconds(0));
};
