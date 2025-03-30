#pragma once
#include "libraries.h"
#include "NoteObject.h"

class RollNote : public NoteObject
{
public:
	RollNote(const MusicalNote* p = nullptr, const chrono::microseconds t = chrono::microseconds(0), int hitCount = 1,
		const MusicalPosition& tail = MusicalPosition(), const chrono::microseconds& tailT = chrono::microseconds(0));
	virtual ~RollNote();

	virtual void Init();
	virtual void OnPass();
	virtual void OnAction(const MilliDouble& refTime, UINT type, UINT act);
	virtual void OnHitInaccurate();
	virtual void OnHitSuccess();
	virtual bool CheckActionAccurate(UINT inputType, UINT actt) const;
	virtual void UpdateScore(ScorePercentage&) const;
	virtual void UpdateScoreOnMiss(ScorePercentage&) const;
	virtual bool IsHitted() const;
	virtual chrono::microseconds NoteEndTime() const;
	virtual const vector<NoteDrawDesc> GetNoteDrawDesc() const;

	virtual void DebugText(wstringstream& wss) const;

	//RollNote information
protected:
	const int maxHitCount;
	int hitCount;
	const MusicalPosition tailPos;
	const chrono::microseconds tailPosTiming;
};
