#include "framework.h"
#include "Lane.h"

void Lane::AddTargetKey(size_t key)
{
	targetNoteKeyList.emplace(key);
}

void Lane::RemoveTargetKey(size_t key)
{
	targetNoteKeyList.erase(key);
}

void Lane::LoadNotes(MusicScore* ptr)
{
	assert(ptr != nullptr);
	
	using NoteContainer = decltype(ptr->notesPerKeyMap);
	NoteContainer& wholeNoteList = ptr->notesPerKeyMap;

	//in case there is no Note which is using current key
	/*
	if musicscroll is using 0~7key and there is keytype 8 in the list,
	you should remove the number 8 from the list
	*/
	
	[&]() //RemoveDuplicatedTargetKey
		{
			for (const size_t& keyType : targetNoteKeyList)
			{
				NoteContainer::const_iterator it = wholeNoteList.find(keyType);
				if (it == wholeNoteList.cend()) RemoveTargetKey(keyType);
			}
		};

	[&]() //CalculateTotalExpectedNotes
		{
			noteList.clear();
			size_t totalExpectedNotes = 0;
			for (const size_t& keyType : targetNoteKeyList)
			{
				//MusicScore::NoteList
				map<MusicalPosition, Note>& currentNoteList = wholeNoteList[keyType];
				totalExpectedNotes += currentNoteList.size();
			}
			noteList.reserve(totalExpectedNotes);
		};

	//object that contains first note of each note
	unordered_map<size_t, NoteContainer::mapped_type::iterator> targetNoteIterList;
	priority_queue<Note*, vector<Note*>, ShortCut::ptrHeapLess<Note>> targetNoteListTimeSort;
	size_t validIterCount = targetNoteKeyList.size();
	for (const size_t& keyType : targetNoteKeyList)
	{
		//MusicScore::NoteList == NoteContainer::mapped_type
		map<MusicalPosition, Note>::iterator firstNoteOfEachKeyIt = wholeNoteList[keyType].begin();

		//end iterator check
		if (firstNoteOfEachKeyIt != wholeNoteList[keyType].end())
		{
			targetNoteIterList.insert(make_pair(keyType, firstNoteOfEachKeyIt));
			targetNoteListTimeSort.push(&(firstNoteOfEachKeyIt->second));
		}
		else --validIterCount;
	}

	while (validIterCount != 0)
	{
		//select most earliest note
		Note* const& mostEarliestNote = targetNoteListTimeSort.top();
		size_t key_of_recentlyPoppedNote = mostEarliestNote->keyType;
		noteList.push_back(mostEarliestNote);
		targetNoteListTimeSort.pop();

		//add new note
		NoteContainer::mapped_type::iterator& nextIt = targetNoteIterList[key_of_recentlyPoppedNote];
		++nextIt;
		if (nextIt != wholeNoteList[key_of_recentlyPoppedNote].end())
		{
			targetNoteListTimeSort.push(&nextIt->second);
		}
		else --validIterCount;
	}
	
}

void Lane::MoveNoteIterator(bool forward)
{
	if (forward)
	{
		if (currentNote != noteList.end()) ++currentNote;
	}
	else
	{
		if (currentNote != noteList.begin()) --currentNote;
	}
}

