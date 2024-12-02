#include "framework.h"
#include "NoteDesc.h"


NoteDesc::NoteDesc(const MusicalNote* p, const chrono::microseconds t, bool pass, bool inaccurate)
	:mNote(p), timing(t), isPassed(pass), isInaccurate(inaccurate), hitCondition(nullptr)
{
}

NoteDesc::~NoteDesc()
{
	delete hitCondition;
}

void NoteDesc::Init()
{
	isPassed = false;
	isInaccurate = false;
	hitCondition->Init();
}

void NoteDesc::OnPass()
{
	isPassed = true;
}

void NoteDesc::SetHitCondition(HitCondition* hc)
{
	delete hitCondition;
	hitCondition = hc;
}
