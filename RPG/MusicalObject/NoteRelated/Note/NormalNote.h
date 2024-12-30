#pragma once
#include "libraries.h"
#include "NoteObject.h"

class NormalNote : public NoteObject
{
public:
	NormalNote(const MusicalNote* p = nullptr, const chrono::microseconds t = chrono::microseconds(0));
	virtual ~NormalNote();

	void Init();
	virtual void OnPass();
	virtual void OnAction(const MilliDouble& refTime, UINT type, UINT act);
	virtual void OnHitInaccurate();
	virtual void OnHitSuccess();
	virtual bool CheckActionAccurate(UINT inputType, UINT act) const;
	virtual void UpdateScore(ScorePercentage&) const;
	virtual void UpdateScoreOnMiss(ScorePercentage&) const;
	virtual bool IsHitted() const;
	virtual void DebugText(wstringstream& wss) const;

	//NormalNote information
protected:
	const bool& IsInaccurate() const { return isInaccurate; }
	void SetInaccurateStatus(bool v) { isInaccurate = v; }
	
	void SetHittedStatus(bool v) { isHitted = v; }

	bool isInaccurate;
	bool isHitted;

};
