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
	SetInaccurateStatus(false);
}

void NoteObject::OnPass()
{
	SetPassedStatus(true);
}

void NoteObject::OnAction(const MilliDouble& refTime, UINT type, UINT act)
{
	const AccuracyRange::Info* const& range = GetAccRange()->GetRangeInfo(refTime, Timing());
	if (range == nullptr) return; //out of range

	sInfo = range->GetInterpolatedScoreInfo(refTime, Timing());

	if (range->id > AccuracyRange::RangeName::Good) //BAD JUDGE
	{
		OnHitInaccurate();
		return;
	}

	if (!CheckNoteType(type, act)) // wrong note type
	{
		sInfo.color = MyColor4::MyRed;
		OnHitInaccurate();
		return;
	}

	// Hit successed, MAX ~ GOOD Judge
	OnHitSuccess();
}

void NoteObject::OnHitInaccurate()
{
	SetInaccurateStatus(true);
}

void NoteObject::OnHitSuccess()
{
	isHitted = true;
}

void NoteObject::AddScore(ScorePercentage& sp) const
{
	if (!IsHitted())
	{
		sp.AddScoreRate(0.0);
		return;
	}
	constexpr double inaccuratePenalty = 0.5;
	const double resultPercentage = IsInaccurate() ? sInfo.percentage * inaccuratePenalty : sInfo.percentage;
	sp.AddScoreRate(resultPercentage);
}

bool NoteObject::IsHitted() const
{
	return isHitted;
}

void NoteObject::DebugText(wstringstream& wss) const
{
	wss << L"Measure: " << MP().measureIdx << "  Pos: " << MP().position.Numerator() << "/" << MP().position.Denominator()
		<< L" Type: " << NoteType() << L" Inaccurate " << ShortCut::BoolToWstr(IsInaccurate());
}

bool NoteObject::CheckNoteType(UINT inputType, UINT act) const
{
	//if (act != (UINT)PlayScene::TaikoActionType::Down) return false;

	switch (NoteType())
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
