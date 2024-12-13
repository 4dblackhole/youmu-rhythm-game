#pragma once
#include "framework.h"
#include "NoteRelated/Note/NoteObject.h"
#include "NoteRelated/Note/NoteDrawDesc.h"

/*to check the order of each notes
if the targetNoteList is {0, 1}, then player should pay attention to the order in which they hit notes 0 and 1
*/

class Lane
{
public:
	using NoteObjectContainer = vector<NoteObject*>;
public:
	Lane() {}
	~Lane();

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
	
	NoteObjectContainer& NoteList() { return noteObjectList; }
	const NoteObjectContainer& NoteListConst() const { return noteObjectList; }
	
	NoteObjectContainer::iterator& CurrentNote() { return currentNote; }
	const NoteObjectContainer::const_iterator& CurrentNoteConst() const { return currentNote; }

	const bool IsNoTargetNote() const { return currentNote == NoteListConst().end(); }

	void MissCurrentNote();
	void HitCurrentNote();

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
	NoteObjectContainer noteObjectList;
	void AddNoteObjectFromNoteTaikoMode(const MusicScore* score, const MusicalNote* const& targetNote);
	void ClearNoteObjectList();
	void InitNoteList();

	set<size_t> targetNoteTypeList;
	void RemoveUnusedNoteType(const MusicScore::NoteContainer& wholeNoteList);
	size_t CalculateTotalExpectedNotes(const MusicScore::NoteContainer& wholeNoteList);

	map<size_t, NoteDrawDesc> noteDrawDescMap;

	NoteObjectContainer::iterator currentNote;

};

class A
{
public:
	A() {};
	~A() {};
	int GetX() const { return x; }
private:
	int x;
};

class B : public A
{
public:
	B();
	~B() {};
};

B::B()
{
	GetX();
}