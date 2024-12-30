#include "framework.h"
#include "NoteObject.h"
#include "GameScene/PlayScene.h"

NoteObject::NoteObject(const MusicalNote* p, const chrono::microseconds t) :
	mp(p->mp), noteType(p->noteType), actionType(p->actionType),
	timing(t)
{
	Init();
}

NoteObject::~NoteObject()
{
}

void NoteObject::Init()
{
	SetPassedStatus(false);
}