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
	//TODO: �巳�� ���� ��� �Ǻ� ���� ���� ��Ʈ�� �������� ���� ���ɼ��� ����
	//���� ���, �巳�Ѱ��� ��� �Ǻ����Ͽ����� ����, �������� ��Ʈ���� �־����µ�
	//�̰��� ������ �����Ϸ��� �巳�� ƽ�� �ϳ��ϳ� �� �߰��ؾ��ϴµ� �ش� �巳���� ƽ��
	//�Ǻ����� �󿡴� ��ü�μ� �������� �ʰ� �Ͻ������θ� ����
	//�׷��Ƿ� �Ǻ���Ʈ�� ���� �����ϴ� ���� �ƴ϶� �ٸ� ����� �ʿ�
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