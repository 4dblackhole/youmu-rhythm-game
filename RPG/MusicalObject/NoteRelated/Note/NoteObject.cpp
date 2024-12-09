#include "framework.h"
#include "NoteObject.h"
#include "GameScene/PlayScene.h"

NoteObject::NoteObject(const MusicalNote* p, const chrono::microseconds t, bool pass, bool inaccurate, int maxC)
	:mNote(p), timing(t), isPassed(pass), isInaccurate(inaccurate), accRange(), maxHitCount(maxC), hitCount(maxC)
{
}

NoteObject::~NoteObject()
{
	//delete hitCondition;
}

void NoteObject::Init()
{
	isPassed = false;
	isInaccurate = false;
	//hitCondition->Init();
	hitCount = maxHitCount;
}

void NoteObject::OnPass()
{
	isPassed = true;
}

void NoteObject::OnAction(const MilliDouble& refTime, UINT type, UINT act)
{
	const AccuracyRange::Info* const& range = GetAccRange()->GetRangeInfo(refTime, Timing());
	if (range == nullptr) return; //out of range

	sInfo = range->GetInterpolatedScoreInfo(refTime, Timing());

	if (range->id > AccuracyRange::RangeName::Good) //BAD JUDGE
	{
		SetInaccurateStatus(true);
		return;
	}

	if (!CheckNoteType(type, act)) // wrong note type
	{
		sInfo.color = MyColor4::MyRed;
		SetInaccurateStatus(true);
		return;
	}

	// Hit successed, MAX ~ GOOD Judge
	OnHit();
}

void NoteObject::OnHit()
{
	//hitCondition->OnHit();
	--hitCount;
}

void NoteObject::AddScore(ScorePercentage& sp) const
{
	constexpr double inaccuratePenalty = 0.5;
	const double resultPercentage = IsInaccurateConst() ? sInfo.percentage * inaccuratePenalty : sInfo.percentage;
	sp.AddScoreRate(resultPercentage);
}

bool NoteObject::IsHitted() const
{
	//return hitCondition->IsHitted();
	return hitCount == 0;
}

/*
void NoteObject::SetHitCondition(HitCondition* hc)
{
	delete hitCondition;
	hitCondition = hc;
}
*/

bool NoteObject::CheckNoteType(UINT inputType, UINT act) const
{
	const UINT noteType = NoteRef()->noteType;
	//if (act != (UINT)PlayScene::TaikoActionType::Down) return false;

	switch (noteType)
	{
	case (UINT)PlayScene::TaikoNoteType::Don:
	case (UINT)PlayScene::TaikoNoteType::BigDon:
		return inputType == (UINT)PlayScene::TaikoInputType::Don;
		break;

	case (UINT)PlayScene::TaikoNoteType::Kat:
	case (UINT)PlayScene::TaikoNoteType::BigKat:
		return inputType == (UINT)PlayScene::TaikoInputType::Kat;
		break;
	}

	return false;
}
