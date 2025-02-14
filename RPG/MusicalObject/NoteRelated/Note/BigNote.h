#pragma once
#include "libraries.h"
#include "NoteObject.h"

class BigNote : public NoteObject
{
public:
	BigNote(const MusicalNote* p = nullptr, const chrono::microseconds t = chrono::microseconds(0));
	virtual ~BigNote();

	void Init();
	virtual void OnPass();
	virtual void OnAction(const MilliDouble& refTime, UINT type, UINT act);
	virtual void OnHitInaccurate();
	virtual void OnHitSuccess();
	virtual bool CheckActionAccurate(UINT inputType, UINT act) const;
	virtual void UpdateScore(ScorePercentage&) const;
	virtual void UpdateScoreOnMiss(ScorePercentage&) const;
	virtual bool IsHitted() const;
	virtual chrono::microseconds NoteEndTime() const;
	virtual const vector<NoteDrawDesc> GetNoteDrawDesc() const;

	virtual void DebugText(wstringstream& wss) const;


	//note information
protected:
	static constexpr int maxHitCount = 2;
	int hitCount;
	bool isInaccurate[BigNote::maxHitCount];
	double score[BigNote::maxHitCount];
};