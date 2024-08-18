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

private:
	set<size_t> targetNoteKeyList;
	vector<Note*> noteList;
	Note* currentNote = nullptr;
};