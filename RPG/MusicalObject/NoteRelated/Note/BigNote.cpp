#include "framework.h"
#include "BigNote.h"
#include "GameScene/PlayScene.h"

BigNote::BigNote(const MusicalNote* p, const chrono::microseconds t) : NoteObject(p, t)
{
	Init();
}

BigNote::~BigNote()
{
}

void BigNote::Init()
{
	hitCount = BigNote::maxHitCount;
	for (double& it : score)it = 0.0;
	for (bool& it : isInaccurate) it = false;
}

void BigNote::OnPass()
{
	SetPassedStatus(true);
}

void BigNote::OnAction(const MilliDouble& refTime, UINT type, UINT act)
{
	const AccuracyRange::Info* const& range = GetAccRange()->GetRangeInfo(refTime, Timing());
	if (range == nullptr) return; //out of range

	sInfo = range->GetInterpolatedScoreInfo(refTime, Timing());

	if (range->id > AccuracyRange::RangeName::Good) //BAD JUDGE
	{
		OnHitInaccurate();
		return;
	}

	if (!CheckActionAccurate(type, act)) // wrong note type
	{
		sInfo.color = MyColor4::MyRed;
		OnHitInaccurate();
		return;
	}

	// Hit successed, MAX ~ GOOD Judge
	OnHitSuccess();
}

void BigNote::OnHitInaccurate()
{
	const int eachBignoteIdx = BigNote::maxHitCount - hitCount;
	isInaccurate[eachBignoteIdx] = true;
}

void BigNote::OnHitSuccess()
{
	const int eachBignoteIdx = BigNote::maxHitCount - hitCount;
	score[eachBignoteIdx] = sInfo.percentage;
	hitCount = max(0, hitCount - 1);
}

bool BigNote::CheckActionAccurate(UINT inputType, UINT act) const
{
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

void BigNote::UpdateScore(ScorePercentage& sp) const
{
	constexpr double inaccuratePenalty = 0.5;

	for (int i = 0; i < BigNote::maxHitCount; ++i)
	{
		const double resultPercentage = isInaccurate[i] ? inaccuratePenalty * score[i] : score[i];
		sp.AddScoreRate(resultPercentage);
	}
}

void BigNote::UpdateScoreOnMiss(ScorePercentage& sp) const
{
	UpdateScore(sp);
}

bool BigNote::IsHitted() const
{
	return hitCount == 0;
}

chrono::microseconds BigNote::NoteEndTime() const
{
	return Timing();
}

void BigNote::DebugText(wstringstream& wss) const
{
	wss << L"Measure: " << MP().measureIdx << "  Pos: " << MP().position.Numerator() << "/" << MP().position.Denominator()
		<< L" Type: " << NoteType() << L" HitCount: " << hitCount 
		<< L" Inaccurate " << ShortCut::BoolToWstr(isInaccurate[0]) <<L" " << ShortCut::BoolToWstr(isInaccurate[1]);
}
