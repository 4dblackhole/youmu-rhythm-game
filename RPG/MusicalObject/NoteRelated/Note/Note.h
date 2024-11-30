#pragma once
#include "libraries.h"
#include "MusicalObject/NoteRelated/HitCondition/HitCondition.h"
#include "MusicalObject/MusicalObjects.h"
#include "MusicalObject/AccuracyRange.h"

class Note
{
public:
	Note(const MusicalPosition& pos, const chrono::microseconds& t);
	virtual ~Note() {}

	virtual void Init();

protected:
	const MusicalPosition mp;
	const chrono::microseconds timing;
	const AccuracyRange* accRange; //weak ptr
	bool isPassed;

public:
	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareLowerBoundPtr(Note* const& s, const MemberType& v)
	{
		return Comparator()(s->timing, v);
	}

	template <typename MemberType, typename Comparator = std::less<MemberType>>
	static bool CompareUpperBoundPtr(const MemberType& v, Note* const& s)
	{
		return Comparator()(v, s->timing);
	}
};
