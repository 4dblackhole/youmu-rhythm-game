#pragma once
#include "libraries.h"
#include "MusicalObject/NoteRelated/HitCondition/HitCondition.h"
#include "MusicalObject/AccuracyRange.h"
#include "MusicalObject/ScorePercentage.h"

class NoteObject
{
public:
	NoteObject(const MusicalNote* p = nullptr, const chrono::microseconds t = chrono::microseconds(0), bool pass = false, bool inaccurate = false, int maxC = 1);
	~NoteObject();

	void Init();
	void OnPass();
	void OnAction(const MilliDouble& refTime, UINT type, UINT act);
	void OnHit();
	void AddScore(ScorePercentage&) const;
	bool IsHitted() const;

	const MusicalNote* NoteRef() const { return mNote; }
	const chrono::microseconds& Timing() const { return timing; }


	const bool& IsPassedConst() const { return isPassed; }
	void SetPassedStatus(bool v) { isPassed = v; }

	const bool& IsInaccurateConst() const { return isInaccurate; }
	void SetInaccurateStatus(bool v) { isInaccurate = v; }

	const int GetHitCount() const { return hitCount; }

	/*
	void SetHitCondition(HitCondition* hc); //move pointer owner
	HitCondition* const& GetHitCondition() const { return hitCondition; }
	*/

	void SetAccRange(const AccuracyRange* const& ptr) { accRange = ptr; }
	const AccuracyRange* const& GetAccRange() const { return accRange; }

	const AccuracyRange::ScoreInfo& GetScoreInfo() const { return sInfo; }

protected:
	//TODO: 드럼롤 같은 경우 악보 파일 상의 노트를 참조하지 않을 가능성이 있음
	//예를 들어, 드럼롤같은 경우 악보파일에서는 시작, 끝범위와 비트수만 주어지는데
	//이것을 실제로 구현하려면 드럼롤 틱을 하나하나 다 추가해야하는데 해당 드럼롤의 틱은
	//악보파일 상에는 실체로서 존재하지 않고 암시적으로만 존재
	//그러므로 악보노트를 직접 참조하는 것이 아니라 다른 방식이 필요
	const MusicalNote* mNote; //weak ptr
	const chrono::microseconds timing;
	bool isPassed;
	bool isInaccurate;
	const int maxHitCount;
	int hitCount;
	
	AccuracyRange::ScoreInfo sInfo;
	
	const AccuracyRange* accRange = nullptr; //weak ptr

private:
	bool CheckNoteType(UINT type, UINT act) const;

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