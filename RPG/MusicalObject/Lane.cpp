#include "framework.h"
#include "Lane.h"

#include "GameScene/PlayScene.h"
#include "MusicalObject/NoteRelated/Note/NormalNote.h"
#include "MusicalObject/NoteRelated/Note/BigNote.h"
#include "MusicalObject/NoteRelated/Note/RollNote.h"

#include "MusicalObject/NoteRelated/NoteFactory/NoteFactory.h"

constexpr float LaneWidth = 180.0f;

void Lane::Reset()
{
	InitNoteList();
	currentNote = noteObjectList.begin();
}

Lane::~Lane()
{
	ClearNoteObjectList();
}

void Lane::OnResize()
{
	laneSprite.OnResize();
	judgeLineSprite.OnResize();
}

void Lane::Update(float dt)
{
	if (laneLightSprite.Diffuse.w > 0)laneLightSprite.Diffuse.w = max(0.0f, laneLightSprite.Diffuse.w - (float)(dt * laneLightFadeSpeed));
}

void Lane::Render(ID3D11DeviceContext* context, const Camera& cam)
{
	laneSprite.Render(context, cam);
	laneLightSprite.Render(context, cam);
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

const NoteDrawDesc& Lane::GetNoteDrawDesc(size_t key) const
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


void Lane::LoadNotesVariablesInit()
{
	isLN = false;
	tempLnHead = nullptr;
	prevNote = nullptr;
}

void Lane::LoadNotes(const MusicScore* score)
{
	assert(score != nullptr);
	
	LoadNotesVariablesInit();

	//noteType[1] - Note[N] List
	const MusicScore::NoteContainer& wholeNoteList = score->notesPerTypeMap;

	RemoveUnusedNoteType(wholeNoteList);
	const size_t& totalExpectedNotes = CalculateTotalExpectedNotes(wholeNoteList);

	ClearNoteObjectList();
	noteObjectList.reserve(totalExpectedNotes);

	//object containing first note of each note
	unordered_map<size_t, MusicScore::NoteContainer::mapped_type::const_iterator> targetNoteIterList;
	priority_queue<const MusicalNote*, vector<const MusicalNote*>, ShortCut::ptrHeapLess<MusicalNote>> targetNoteListTimeSort;
	size_t validIterCount = targetNoteTypeList.size();
	for (const size_t& keyType : targetNoteTypeList)
	{
		//insert the first note of each types into p-Queue 
		const bool insertResult = [&]() -> bool
			{
				const MusicScore::NoteContainer::const_iterator noteTypeList = wholeNoteList.find(keyType);
				if (noteTypeList == wholeNoteList.cend()) return false; //there is no notes of the type

				const map<MusicalPosition, MusicalNote>::const_iterator firstNoteOfTypeIt = (*noteTypeList).second.begin();

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
		const MusicalNote* const& mostEarliestNote = targetNoteListTimeSort.top();
		AddNoteObjectFromNoteTaikoMode(score, mostEarliestNote);

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

	currentNote = noteObjectList.begin();

}


void Lane::InitNoteList()
{
	for (NoteObjectContainer::value_type& it : noteObjectList) it->Init();
	
}

void Lane::AddNoteObjectFromNoteTaikoMode(const MusicScore* score, const MusicalNote* const& targetNote)
{
	if (targetNote == nullptr) return;

	const chrono::microseconds& timing = score->GetNoteTimingPoint(targetNote->mp);

	UINT noteType = targetNote->noteType;
	UINT actType = targetNote->actionType;
	
	NoteObject* notePtr = nullptr;
	
	switch (actType)
	{
	case (UINT)PlayScene::TaikoActionType::Down:
	{
		if (isLN) return;

		NoteObject* const notePtr = NoteFactory::CreateTaikoNote(noteType, actType, targetNote, timing);
		if (notePtr == nullptr) return; // in case dynamic allocation failure

		noteObjectList.emplace_back(notePtr);
		NoteObject& lastNote = *noteObjectList.back();
		lastNote.SetAccRange(&score->accRange);
		return;
	}
	case (UINT)PlayScene::TaikoActionType::LNStart:
	{
		if (isLN) return;

		isLN = true;
		tempLnHead = targetNote;
		tempLnHeadTiming = timing;
	}
	return;
	case (UINT)PlayScene::TaikoActionType::LNEnd:
	{
		if (!isLN) return;
		if (tempLnHead->noteType != noteType) return;
		isLN = false;

		switch (noteType)
		{
		case (UINT)PlayScene::TaikoNoteType::Roll:
			notePtr = new RollNote(tempLnHead, tempLnHeadTiming, 10, targetNote->mp, timing);
			break;
		}

		if (notePtr != nullptr)
		{
			noteObjectList.emplace_back(notePtr);
			NoteObject& lastNote = *noteObjectList.back();
			lastNote.SetAccRange(&score->accRange);
		}
	}
	return;

	}
}

void Lane::ClearNoteObjectList()
{
	if (noteObjectList.empty()) return;
	for (NoteObject*& it : noteObjectList) delete it;
	noteObjectList.clear();
}

void Lane::RemoveUnusedNoteType(const MusicScore::NoteContainer& wholeNoteList)
{
	//in case there is no Note which is using current type
	/*
	when musicscroll is using 0~7key and there is keytype 8 in the list,
	i decided to remove the number 8 from the list
	*/
	vector<size_t> deleteTypeList;
	deleteTypeList.reserve(targetNoteTypeList.size());
	for (const size_t& noteType : targetNoteTypeList)
	{
		MusicScore::NoteContainer::const_iterator it = wholeNoteList.find(noteType);
		if (it == wholeNoteList.cend()) deleteTypeList.emplace_back(noteType);
	}

	for (const size_t& noteType : deleteTypeList) RemoveNoteType(noteType);
	
}

size_t Lane::CalculateTotalExpectedNotes(const MusicScore::NoteContainer& wholeNoteList)
{
	size_t totalExpectedNotes = 0;
	for (const size_t& keyType : targetNoteTypeList)
	{
		//MusicScore::NoteList
		const map<MusicalPosition, MusicalNote>& currentNoteList = wholeNoteList.at(keyType);
		totalExpectedNotes += currentNoteList.size();
	}
	return totalExpectedNotes;
}

void Lane::UpdateScore(ScorePercentage& sp)
{
	if ((*currentNote)->IsHitted()) (*currentNote)->UpdateScore(sp);
	else (*currentNote)->UpdateScoreOnMiss(sp);
}

void Lane::MoveCurrentNoteForward()
{
	(*currentNote)->OnPass();
	++currentNote;
}

void Lane::MoveCurrentNoteBackward()
{
	--currentNote;
	(*currentNote)->Init();
}

double Lane::GetJudgePosition() const
{
	return judgePosition;
}

void Lane::SetJudgePosition(double val)
{
	judgePosition = val;
}
