#pragma once
#include "framework.h"

/*to check the order of each notes
if the targetNoteList is {0, 1}, then player should pay attention to the order in which they hit notes 0 and 1
*/

class Lane
{
public:
	Lane() {}
	~Lane() {}

	void AddTargetKey(size_t key);
	void RemoveTargetKey(size_t key);

	void LoadNotes(MusicScore* ptr);
	void LoadNoteTimings(const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum);

	const set<size_t>& GetTargetNoteTypeList() const { return targetNoteKeyList; }
	
	vector<chrono::microseconds>& NoteTimingList() { return noteTimingList; }
	vector<Note*>::iterator& CurrentNote() { return currentNote; }
	void MoveNoteIterator(bool forward);

private:
	vector<Note*> noteList; //weak ptr container
	vector<chrono::microseconds> noteTimingList;

	void LoadNoteTiming(const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum, size_t noteIdx);

	set<size_t> targetNoteKeyList;
	vector<Note*>::iterator currentNote;
	
};