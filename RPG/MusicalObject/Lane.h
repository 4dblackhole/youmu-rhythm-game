#pragma once
#include "framework.h"
#include "NoteRelated/Note/NoteDesc.h"
#include "NoteRelated/Note/NoteDrawDesc.h"
#include "NoteRelated/HitCondition/HitCondition.h"

/*to check the order of each notes
if the targetNoteList is {0, 1}, then player should pay attention to the order in which they hit notes 0 and 1
*/

class Lane
{
public:
	Lane() {}
	~Lane() {}

	void OnResize();
	void Update(float dt);
	void Render(ID3D11DeviceContext*, const Camera&);
	void Reset();

	void AddNoteType(size_t key);
	void RemoveNoteType(size_t key);

	const NoteDrawDesc& GetNoteDrawDesc(size_t key) const;
	void AddNoteDrawDesc(size_t key, const NoteDrawDesc& desc);
	void RemoveNoteDrawDesc(size_t key);

	void LoadNotes(const MusicScore* score);

	const set<size_t>& GetTargetNoteTypeList() const { return targetNoteTypeList; }
	
	vector<NoteDesc>& NoteList() { return noteList; }
	const vector<NoteDesc>& NoteListConst() const { return noteList; }
	
	vector<NoteDesc>::iterator& CurrentNote() { return currentNote; }
	const vector<NoteDesc>::const_iterator& CurrentNoteConst() const { return currentNote; }

	void MoveCurrentNoteForward();
	void MoveCurrentNoteBackward();

	Sprite laneSprite;
	Sprite judgeLineSprite;
	Sprite laneLightSprite;
	static constexpr double laneLightFadeSpeed = 8.0;

	void ChangeLaneLightColor(const XMFLOAT4& color = MyColor4::White);

public:
	double GetJudgePosition() const;
	void SetJudgePosition(double val);
private:
	double judgePosition = 0.0;

private:
	vector<NoteDesc> noteList;
	void AddNoteDescFromNoteTaikoMode(const MusicScore* score, const MusicalNote* const& targetNote);
	void SetNotePassStatus(bool v);
	void SetNoteInaccurate(bool v);
	void InitNoteHitCount();
	HitCondition* GetNoteHitConditionFromType(const MusicalNote& targetNote);

	set<size_t> targetNoteTypeList;
	void RemoveUnusedNoteType(const MusicScore::NoteContainer& wholeNoteList);
	void CalculateTotalExpectedNotes(const MusicScore::NoteContainer& wholeNoteList);

	map<size_t, NoteDrawDesc> noteDrawDescMap;

	vector<NoteDesc>::iterator currentNote;

};