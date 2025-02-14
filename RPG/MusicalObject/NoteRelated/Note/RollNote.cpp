#include "framework.h"
#include "RollNote.h"
#include "GameScene/PlayScene.h"

RollNote::RollNote(const MusicalNote* p, const chrono::microseconds t, int hitCount, const MusicalPosition& tail, const chrono::microseconds& tailT) :
	NoteObject(p, t), maxHitCount(hitCount), tailPos(tail), tailPosTiming(tailT)
{
	Init();
}

RollNote::~RollNote()
{
}

void RollNote::Init()
{
	AccuracyRange asdf;
	accRange = &asdf;
	hitCount = maxHitCount;
}

void RollNote::OnPass()
{
}

void RollNote::OnAction(const MilliDouble& refTime, UINT type, UINT act)
{
	using namespace chrono;
	const microseconds refTimeUs = microseconds((microseconds::rep)duration_cast<MicroDouble>(refTime).count());

	//sInfo.color = MyColor4::White;

	//only key-down action
	if (act != (UINT)PlayScene::TaikoActionType::Down) return;

	// Hit successed
	if (Timing() <= refTimeUs && Timing() <= tailPosTiming) OnHitSuccess();
}

void RollNote::OnHitInaccurate()
{
	//no inaccurate action
}

void RollNote::OnHitSuccess()
{
	hitCount = max(0, hitCount - 1);
}

bool RollNote::CheckActionAccurate(UINT inputType, UINT act) const
{
	return true;
}

void RollNote::UpdateScore(ScorePercentage& sp) const
{
	double hitSuccessRate = (double)(maxHitCount - hitCount) / (double)maxHitCount;
	sp.AddScoreRate(hitSuccessRate);
}

void RollNote::UpdateScoreOnMiss(ScorePercentage& sp) const
{
	UpdateScore(sp);
}

bool RollNote::IsHitted() const
{
	enum HitFlagDesc
	{
		KeepFocus,
		DisposeDirectly
	};
	constexpr UINT hitFlag = HitFlagDesc::KeepFocus;
	if constexpr (hitFlag == HitFlagDesc::KeepFocus) return false;
	else return hitCount == 0;
}

chrono::microseconds RollNote::NoteEndTime() const
{
	return tailPosTiming;
}

const vector<NoteDrawDesc> RollNote::GetNoteDrawDesc() const
{
	return vector<NoteDrawDesc>();
}

void RollNote::DebugText(wstringstream& wss) const
{
	wss << L"Head Pos: " << MP().measureIdx << L", " << MP().position.Numerator() << "/" << MP().position.Denominator()
		<< L"\nTail Pos: " << tailPos.measureIdx << L", " << tailPos.position.Numerator() << "/" << tailPos.position.Denominator()
		<< L" Type: " << NoteType() << L" HitCount: " << hitCount << L"/" << maxHitCount;
}
