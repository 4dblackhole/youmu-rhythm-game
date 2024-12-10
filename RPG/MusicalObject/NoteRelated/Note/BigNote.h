#pragma once
#include "libraries.h"
#include "NoteObject.h"

class BigNote : public NoteObject
{
public:
	BigNote(const MusicalNote* p = nullptr, const chrono::microseconds t = chrono::microseconds(0));
	virtual ~BigNote();

	virtual void Init();
	virtual void OnPass();
	virtual void OnAction(const MilliDouble& refTime, UINT type, UINT act);
	virtual void OnHitInaccurate();
	virtual void OnHitSuccess();
	virtual void AddScore(ScorePercentage&) const;
	virtual bool IsHitted() const;

	virtual void DebugText(wstringstream& wss) const;


	//normal note information
public:
	const bool& IsInaccurate() const { return isInaccurate; }

private:
	static constexpr int maxHitCount = 2;
	int hitCount;
	void SetInaccurateStatus(bool v, int idx) { isInaccurate[idx] = v; }
	bool isInaccurate[2];
};