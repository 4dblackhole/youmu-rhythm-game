#include "framework.h"
#include "Lane.h"

#include "GameScene/PlayScene.h"

void Lane::AddNoteType(size_t key)
{
	targetNoteTypeList.emplace(key);
}

void Lane::RemoveNoteType(size_t key)
{
	targetNoteTypeList.erase(key);
}

const Lane::NoteDrawDesc& Lane::GetNoteDrawDesc(size_t key) const
{
	auto it = noteDrawDescMap.find(key);
	if (it != noteDrawDescMap.end()) return it->second;
	else return defaultNoteDrawDesc;
}

void Lane::AddNoteDrawDesc(size_t key, const NoteDrawDesc& val)
{
	noteDrawDescMap.emplace(make_pair(key, val));
}

void Lane::RemoveNoteDrawDesc(size_t key)
{
	noteDrawDescMap.erase(key);
}

void Lane::LoadNotes(MusicScore* ptr, const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum)
{
	assert(ptr != nullptr);
	
	using NoteContainer = decltype(ptr->notesPerTypeMap);
	NoteContainer& wholeNoteList = ptr->notesPerTypeMap;

	//in case there is no Note which is using current key
	/*
	if musicscroll is using 0~7key and there is keytype 8 in the list,
	you should remove the number 8 from the list
	*/
	
	[&]() //RemoveDuplicatedTargetKey
		{
			for (const size_t& keyType : targetNoteTypeList)
			{
				NoteContainer::const_iterator it = wholeNoteList.find(keyType);
				if (it == wholeNoteList.cend()) RemoveNoteType(keyType);
			}
		};

	[&]() //CalculateTotalExpectedNotes
		{
			noteList.clear();
			size_t totalExpectedNotes = 0;
			for (const size_t& keyType : targetNoteTypeList)
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
	size_t validIterCount = targetNoteTypeList.size();
	for (const size_t& keyType : targetNoteTypeList)
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
		size_t key_of_recentlyPoppedNote = mostEarliestNote->noteType;
		chrono::microseconds timing = Lane::GetNoteTimingPoint(measureSum, bpmSum, *mostEarliestNote);
		noteList.push_back(NoteDesc{ mostEarliestNote, timing, true });
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

	currentNote = noteList.begin();

}

void Lane::InitNoteTimingThread(const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum, size_t noteIdx)
{
	noteList[noteIdx].timing = Lane::GetNoteTimingPoint(measureSum, bpmSum, *noteList[noteIdx].note);
}


void Lane::InitNoteTimings(const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum)
{
	const size_t& noteListSize = noteList.size();
	vector<thread> noteTimingThread;
	noteTimingThread.reserve(noteListSize);
	for (int noteIdx = 0; noteIdx < (int)noteListSize; ++noteIdx)
	{
		noteTimingThread.push_back(thread(&Lane::InitNoteTimingThread, this, measureSum, bpmSum, (size_t)noteIdx));
	}

	for (thread& it : noteTimingThread) it.join();
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

chrono::microseconds Lane::GetNoteTimingPoint(const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum, const Note& note)
{
	using namespace chrono;
	const BpmTimingPrefixSum::BpmPrefixSumContainer::const_iterator& bpmIter = bpmSum.GetBpmTimingPoint(note.mp);
	const MusicalPosition& bpmPos = bpmIter->first;
	const MilliDouble& bpmTiming = bpmIter->second;

	const RationalNumber<64>& relativePos = measureSum.GetMeasurePrefixSum((int)bpmPos.measureIdx, (int)note.mp.measureIdx - 1) + note.mp.position - bpmPos.position;
	const MilliDouble& resultTiming = bpmTiming + MilliDouble((double)relativePos);
	return microseconds((microseconds::rep)duration_cast<duration<double, std::micro>>(resultTiming).count());

}