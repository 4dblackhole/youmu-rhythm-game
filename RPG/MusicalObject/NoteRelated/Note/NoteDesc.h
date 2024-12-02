#pragma once
#include "libraries.h"
#include "Note.h"

class NoteDesc
{
public:
	NoteDesc(const MusicalNote* p = nullptr, const chrono::microseconds t = chrono::microseconds(0), bool pass = false, bool inaccurate = false);
	~NoteDesc();

	void Init();
	void OnPass();

	const MusicalNote* NoteRef() const { return mNote; }
	const chrono::microseconds& Timing() const { return timing; }


	const bool& IsPassedConst() const { return isPassed; }
	void SetPassedStatus(bool v) { isPassed = v; }

	const bool& IsInaccurateConst() const { return isInaccurate; }
	void SetInaccurateStatus(bool v) { isInaccurate = v; }

	void ResetHitCondition() { hitCondition->Init(); };

	void SetHitCondition(HitCondition* hc); //move pointer owner
	HitCondition* const& GetHitCondition() const { return hitCondition; }

private:
	const MusicalNote* mNote; //weak ptr
	const chrono::microseconds timing;
	bool isPassed;
	bool isInaccurate;
	HitCondition* hitCondition;

public:
	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareLowerBound(const NoteDesc& s, const MemberType& v)
	{
		return Comparator()(s.timing, v);
	}

	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareUpperBound(const MemberType& v, const NoteDesc& s)
	{
		return Comparator()(v, s.timing);
	}

	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareLowerBoundPtr(NoteDesc* const& s, const MemberType& v)
	{
		return Comparator()(s->timing, v);
	}

	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareUpperBoundPtr(const MemberType& v, NoteDesc* const& s)
	{
		return Comparator()(v, s->timing);
	}

};