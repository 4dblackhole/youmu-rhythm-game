#pragma once
#include "libraries.h"
#include "MusicalObject/AccuracyRange.h"
#include "MusicalObject/ScorePercentage.h"
#include "NoteDrawDesc.h"

class NoteObject
{
public:
	NoteObject(const MusicalNote* p = nullptr, const chrono::microseconds t = chrono::microseconds(0));
	virtual ~NoteObject();

	void Init();
	virtual void OnPass() = 0;
	virtual void OnAction(const MilliDouble& refTime, UINT type, UINT act) = 0;
	virtual void OnHitInaccurate() = 0;
	virtual void OnHitSuccess() = 0; //is called when player hit the note
	virtual bool CheckActionAccurate(UINT inputType, UINT act) const = 0; //private function?
	virtual void UpdateScore(ScorePercentage&) const = 0; //is called when the player hitted the note and note condition is fulfilled
	virtual void UpdateScoreOnMiss(ScorePercentage&) const = 0; //is called when player missed the note
	virtual bool IsHitted() const = 0;
	virtual chrono::microseconds NoteEndTime() const = 0;
	virtual const vector<NoteDrawDesc> GetNoteDrawDesc() const = 0;

	virtual void DebugText(wstringstream& wss) const = 0;

	const MusicalPosition& MP() const { return mp; }
	const UINT NoteType() const { return noteType; }
	const UINT ActionType() const { return actionType; }
	const vector<FMOD::Sound*>& HitSounds() const { return hitSound; }
	const chrono::microseconds& Timing() const { return timing; }
	
	const bool& IsPassed() const { return isPassed; }
	void SetPassedStatus(bool v) { isPassed = v; }
	
	const AccuracyRange* const& GetAccRange() const { return accRange; }
	void SetAccRange(const AccuracyRange* const& ptr) { accRange = ptr; }
	
	const AccuracyRange::ScoreInfo& GetScoreInfo() const { return sInfo; }

//common information
protected:
	const MusicalPosition mp;
	const UINT noteType = 0;
	const UINT actionType = 0;
	const chrono::microseconds timing;
	const AccuracyRange* accRange = nullptr; //weak ptr
	vector<FMOD::Sound*> hitSound; //hitsound weak ptr list

	bool isPassed;
	AccuracyRange::ScoreInfo sInfo; //change to xmfloat4?

//compare function
public:

	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareLowerBoundPtr(NoteObject* const& s, const MemberType& v)
	{
		return Comparator()(s->NoteEndTime(), v);
	}

	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareUpperBoundPtr(const MemberType& v, NoteObject* const& s)
	{
		return Comparator()(v, s->NoteEndTime());
	}

};