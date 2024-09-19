#include "framework.h"
#include "Lane.h"

#include "GameScene/PlayScene.h"

constexpr float LaneWidth = 180.0f;

void Lane::Init()
{
	laneSprite.GetWorld3d().SetObjectScale({ LaneWidth, (float)LaneMaxLength });
	laneSprite.GetWorld3d().SetCenterPosition({ 0, 0.5f, 0 });
	laneSprite.GetWorld3d().SetLocalRotation({ 0, 0, -Math::PI * 0.5f });
	laneSprite.GetWorld3d().SetLocalPosition({ LaneWidth,100,0 });
	laneSprite.GetWorld3d().SetAlignX(AlignModeX::Left);
	laneSprite.GetWorld3d().SetParentDrawWorld();
	judgePosition = 72.0;

	judgeLineSprite.GetWorld3d().SetParentWorld(&laneSprite.GetWorld3d());
	judgeLineSprite.GetWorld3d().SetLocalPosition({ 0, (float)judgePosition, 0 });
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
	
	using NoteContainer = decltype(score->notesPerTypeMap);
	const NoteContainer& wholeNoteList = score->notesPerTypeMap;

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
				const map<MusicalPosition, Note>& currentNoteList = wholeNoteList.at(keyType);
				totalExpectedNotes += currentNoteList.size();
			}
			noteList.reserve(totalExpectedNotes);
		};

	//object containing first note of each note
	unordered_map<size_t, NoteContainer::mapped_type::const_iterator> targetNoteIterList;

	priority_queue<const Note*, vector<const Note*>, ShortCut::ptrHeapLess<Note>> targetNoteListTimeSort;
	size_t validIterCount = targetNoteTypeList.size();
	for (const size_t& keyType : targetNoteTypeList)
	{
		try
		{
			//MusicScore::NoteList == NoteContainer::mapped_type
			const NoteContainer::const_iterator keyNoteList = wholeNoteList.find(keyType);
			if (keyNoteList == wholeNoteList.cend()) throw nullptr; //there is no notes of the key type

			const map<MusicalPosition, Note>::const_iterator firstNoteOfEachTypeIt = (*keyNoteList).second.begin();

			//end iterator check
			if (firstNoteOfEachTypeIt == wholeNoteList.at(keyType).cend()) throw nullptr;

			targetNoteIterList.insert(make_pair(keyType, firstNoteOfEachTypeIt));
			targetNoteListTimeSort.emplace(&(firstNoteOfEachTypeIt->second));
		}
		catch (void* v)
		{
			UNREFERENCED_PARAMETER(v);
			--validIterCount;
		}
	}

	while (validIterCount != 0)
	{
		//select most earliest note
		const Note* const& mostEarliestNote = targetNoteListTimeSort.top();
		size_t key_of_recentlyPoppedNote = mostEarliestNote->noteType;
		const chrono::microseconds& timing = score->GetNoteTimingPoint(mostEarliestNote->mp);
		noteList.push_back(NoteDesc{ mostEarliestNote, timing, true });

		targetNoteListTimeSort.pop();

		//add new note
		NoteContainer::mapped_type::const_iterator& nextIt = targetNoteIterList[key_of_recentlyPoppedNote];
		++nextIt;
		if (nextIt != wholeNoteList.at(key_of_recentlyPoppedNote).end())
		{
			targetNoteListTimeSort.push(&nextIt->second);
		}
		else --validIterCount;
	}

	currentNote = noteList.begin();

}

void Lane::InitNoteTimings(const MusicScore& score)
{
	const size_t& noteListSize = noteList.size();
	for (int noteIdx = 0; noteIdx < (int)noteListSize; ++noteIdx)
	{
		noteList[noteIdx].timing = score.GetNoteTimingPoint(noteList[noteIdx].note->mp);
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

double Lane::GetJudgePosition() const
{
	return judgePosition;
}

void Lane::SetJudgePosition(double val)
{
	judgePosition = val;
}
