#include "framework.h"
#include "Note.h"

Note::Note(const MusicalPosition& pos, const chrono::microseconds& t)
	:mp(pos), timing(t)
{
	Init();
}

void Note::Init()
{
	accRange = nullptr;
	isPassed = false;
}
