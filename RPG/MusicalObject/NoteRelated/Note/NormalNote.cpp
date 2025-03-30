#include "framework.h"
#include "NormalNote.h"
#include "GameScene/PlayScene.h"

constexpr float CircleDiameter = 90.0f;

NormalNote::NormalNote(const MusicalNote* p, const chrono::microseconds t) : NoteObject(p, t)
{
}

NormalNote::~NormalNote()
{
}

void NormalNote::Init()
{
	__super::Init();
	SetInaccurateStatus(false);
	SetHittedStatus(false);
}

void NormalNote::OnPass()
{
	SetPassedStatus(true);
}

void NormalNote::OnAction(const MilliDouble& refTime, UINT type, UINT act)
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

void NormalNote::OnHitInaccurate()
{
	isInaccurate = true;
}

void NormalNote::OnHitSuccess()
{
	isHitted = true;
}

void NormalNote::UpdateScore(ScorePercentage& sp) const
{
	constexpr double inaccuratePenalty = 0.5;
	const double resultPercentage = isInaccurate ? sInfo.percentage * inaccuratePenalty : sInfo.percentage;
	sp.AddScoreRate(resultPercentage);
}

void NormalNote::UpdateScoreOnMiss(ScorePercentage& sp) const
{
	sp.AddScoreRate(0.0);
}

bool NormalNote::IsHitted() const
{
	return isHitted;
}

chrono::microseconds NormalNote::NoteEndTime() const
{
	return Timing();
}

const vector<NoteDrawDesc> NormalNote::GetNoteDrawDesc() const
{
	static constexpr NoteDrawDesc RedNoteDrawDesc
	(
		{
			MyColor4::MyRed,
			CircleDiameter,
			(UINT)PlayScene::NoteTextureArrID::Note,
			(UINT)PlayScene::NoteTextureArrID::NoteOverlay
		}
	);
	static constexpr NoteDrawDesc BlueNoteDrawDesc
	(
		{
			MyColor4::MyBlue,
			CircleDiameter,
			(UINT)PlayScene::NoteTextureArrID::Note,
			(UINT)PlayScene::NoteTextureArrID::NoteOverlay
		}
	);

	switch (noteType)
	{
	case (UINT)PlayScene::TaikoNoteType::Don:
		return { RedNoteDrawDesc };

	case (UINT)PlayScene::TaikoNoteType::Kat:
		return { BlueNoteDrawDesc };

	default:
		return vector<NoteDrawDesc>();
	}
}

void NormalNote::DebugText(wstringstream& wss) const
{
	wss << L"Measure: " << MP().measureIdx << "  Pos: " << MP().position.Numerator() << "/" << MP().position.Denominator()
		<< L" Type: " << NoteType() << L" Inaccurate " << ShortCut::BoolToWstr(IsInaccurate());
}

bool NormalNote::CheckActionAccurate(UINT inputType, UINT act) const
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

