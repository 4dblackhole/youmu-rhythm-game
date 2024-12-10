#pragma once
#include "libraries.h"
#include "MusicalObject/AccuracyRange.h"
#include "MusicalObject/ScorePercentage.h"

class NoteObject
{
public:
	NoteObject(const MusicalNote* p = nullptr, const chrono::microseconds t = chrono::microseconds(0));
	virtual ~NoteObject();

	virtual void Init();
	virtual void OnPass();
	virtual void OnAction(const MilliDouble& refTime, UINT type, UINT act);
	virtual void OnHitInaccurate();
	virtual void OnHitSuccess();
	virtual void AddScore(ScorePercentage&) const; //is called when player miss/hit the note
	virtual bool IsHitted() const;

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

public:
	virtual void DebugText(wstringstream& wss) const;

//common information
protected:
	const MusicalPosition mp;
	const UINT noteType = 0;
	const UINT actionType = 0;
	const chrono::microseconds timing;
	const AccuracyRange* accRange = nullptr; //weak ptr
	vector<FMOD::Sound*> hitSound; //hitsound weak ptr list

	bool isPassed;
	AccuracyRange::ScoreInfo sInfo;
protected:
	bool CheckNoteType(UINT type, UINT act) const;

//normal note information
public:
	const bool& IsInaccurate() const { return isInaccurate; }

private:
	void SetInaccurateStatus(bool v) { isInaccurate = v; }

	bool isInaccurate;
	bool isHitted = false;

//compare function
public:
	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareLowerBound(const NoteObject& s, const MemberType& v)
	{
		return Comparator()(s.timing, v);
	}

	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareUpperBound(const MemberType& v, const NoteObject& s)
	{
		return Comparator()(v, s.timing);
	}

	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareLowerBoundPtr(NoteObject* const& s, const MemberType& v)
	{
		return Comparator()(s->timing, v);
	}

	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareUpperBoundPtr(const MemberType& v, NoteObject* const& s)
	{
		return Comparator()(v, s->timing);
	}

};