#include "framework.h"
#include "BigNote.h"
#include "GameScene/PlayScene.h"

BigNote::BigNote(const MusicalNote* p, const chrono::microseconds t) : NoteObject(p, t)
{
	this->Init();
}

BigNote::~BigNote()
{
}

void BigNote::Init()
{
	hitCount = BigNote::maxHitCount;
}

void BigNote::OnPass()
{
}

void BigNote::OnAction(const MilliDouble& refTime, UINT type, UINT act)
{
}

void BigNote::OnHitInaccurate()
{
	SetInaccurateStatus(true, BigNote::maxHitCount - hitCount);
}

void BigNote::OnHitSuccess()
{
	hitCount = max(0, hitCount - 1);
}

void BigNote::AddScore(ScorePercentage&) const
{
}

bool BigNote::IsHitted() const
{
	return hitCount == 0;
}

void BigNote::DebugText(wstringstream& wss) const
{
}
