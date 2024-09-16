#pragma once
#include "framework.h"

/*to check the order of each notes
if the targetNoteList is {0, 1}, then player should pay attention to the order in which they hit notes 0 and 1
*/

class Lane
{
public:
	struct NoteDesc
	{
	public:
		Note* note; //weak ptr
		chrono::microseconds timing;
		bool visible;
	};
	
	struct NoteDrawDesc
	{
	public:
		XMFLOAT4 color;
		double diameter;
	};
	static constexpr NoteDrawDesc defaultNoteDrawDesc{ MyColor4::Blank, 0 };

public:
	Lane() {}
	~Lane() {}

	void AddNoteType(size_t key);
	void RemoveNoteType(size_t key);

	const NoteDrawDesc& GetNoteDrawDesc(size_t key) const;
	void AddNoteDrawDesc(size_t key, const NoteDrawDesc& desc);
	void RemoveNoteDrawDesc(size_t key);

	void LoadNotes(MusicScore* ptr, const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum);

	const set<size_t>& GetTargetNoteTypeList() const { return targetNoteTypeList; }
	
	vector<NoteDesc>& NoteList() { return noteList; }
	const vector<NoteDesc>& NoteListConst() const { return noteList; }

	vector<NoteDesc>::iterator& CurrentNote() { return currentNote; }
	const vector<NoteDesc>::iterator& CurrentNoteConst() const { return currentNote; }

	void MoveNoteIterator(bool forward);

	double HitPosition = 0.0;

	Sprite Sprite;

private:
	vector<NoteDesc> noteList; //weak ptr container

	void InitNoteTimings(const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum);
	void InitNoteTimingThread(const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum, size_t noteIdx);

	set<size_t> targetNoteTypeList;
	map<size_t, NoteDrawDesc> noteDrawDescMap;

	vector<NoteDesc>::iterator currentNote;

public:
	//TODO: Change the owner of this function
	static chrono::microseconds GetNoteTimingPoint(const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum, const Note& note);
};