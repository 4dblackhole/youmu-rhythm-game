#include "framework.h"
#include "Lane.h"

#include "GameScene/PlayScene.h"

constexpr float LaneWidth = 180.0f;

void Lane::Reset()
{
	ResetNoteVisible();
	currentNote = noteList.begin();
}

void Lane::OnResize()
{
	laneSprite.OnResize();
	judgeLineSprite.OnResize();
}

void Lane::Update(float dt)
{
}

void Lane::Render(ID3D11DeviceContext* context, const Camera& cam)
{
	laneSprite.Render(context, cam);
	judgeLineSprite.Render(context, cam);
}

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

void Lane::LoadNotes(const MusicScore* score)
{
	assert(score != nullptr);
	
	//noteType[1] - Note[N] List
	const MusicScore::NoteContainer& wholeNoteList = score->notesPerTypeMap;

	RemoveDuplicatedTargetKey(wholeNoteList);
	CalculateTotalExpectedNotes(wholeNoteList);

	//object containing first note of each note
	unordered_map<size_t, MusicScore::NoteContainer::mapped_type::const_iterator> targetNoteIterList;
	priority_queue<const Note*, vector<const Note*>, ShortCut::ptrHeapLess<Note>> targetNoteListTimeSort;
	size_t validIterCount = targetNoteTypeList.size();
	for (const size_t& keyType : targetNoteTypeList)
	{
		//insert the first note of each types into p-Queue 
		const bool insertResult = [&]() -> bool
			{
				const MusicScore::NoteContainer::const_iterator noteTypeList = wholeNoteList.find(keyType);
				if (noteTypeList == wholeNoteList.cend()) return false; //there is no notes of the type

				const map<MusicalPosition, Note>::const_iterator firstNoteOfTypeIt = (*noteTypeList).second.begin();

				//in case there is no note
				if (firstNoteOfTypeIt == wholeNoteList.at(keyType).cend()) return false;

				targetNoteIterList.emplace(make_pair(keyType, firstNoteOfTypeIt));
				targetNoteListTimeSort.emplace(&(firstNoteOfTypeIt->second));

				return true;
			}();

		if (insertResult == false)--validIterCount;
	}

	while (validIterCount != 0)
	{
		//select most earliest note
		const Note* const& mostEarliestNote = targetNoteListTimeSort.top();
		AddNoteDescFromNote(score, mostEarliestNote);

		//add new note
		const size_t& key_of_recentlyPoppedNote = mostEarliestNote->noteType; 
		MusicScore::NoteContainer::mapped_type::const_iterator& nextIt = targetNoteIterList[key_of_recentlyPoppedNote];
		targetNoteListTimeSort.pop();
		++nextIt;
		if (nextIt != wholeNoteList.at(key_of_recentlyPoppedNote).end())
		{
			targetNoteListTimeSort.push(&nextIt->second);
		}
		else --validIterCount;
	}

	currentNote = noteList.begin();

}

void Lane::ResetNoteVisible()
{
	for (auto& it : noteList)it.visible = true;
}

void Lane::AddNoteDescFromNote(const MusicScore* score, const Note* const& targetNote)
{
	const size_t& key_of_recentlyPoppedNote = targetNote->noteType;
	const chrono::microseconds& timing = score->GetNoteTimingPoint(targetNote->mp);
	noteList.emplace_back(NoteDesc{ targetNote, timing, true });
}

void Lane::RemoveDuplicatedTargetKey(const MusicScore::NoteContainer& wholeNoteList)
{
	//in case there is no Note which is using current key
	/*
	if musicscroll is using 0~7key and there is keytype 8 in the list,
	you should remove the number 8 from the list
	*/
	for (const size_t& keyType : targetNoteTypeList)
	{
		MusicScore::NoteContainer::const_iterator it = wholeNoteList.find(keyType);
		if (it == wholeNoteList.cend()) RemoveNoteType(keyType);
	}
}

void Lane::CalculateTotalExpectedNotes(const MusicScore::NoteContainer& wholeNoteList)
{
	noteList.clear();
	size_t totalExpectedNotes = 0;
	for (const size_t& keyType : targetNoteTypeList)
	{
		//MusicScore::NoteList
		const map<MusicalPosition, Note>& currentNoteList = wholeNoteList.at(keyType);
		totalExpectedNotes += currentNoteList.size();
	}
	noteList.reserve(totalExpectedNotes);
}

double Lane::GetJudgePosition() const
{
	return judgePosition;
}

void Lane::SetJudgePosition(double val)
{
	judgePosition = val;
}