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
		const Note* note; //weak ptr
		const chrono::microseconds timing;
		bool visible;

		template <typename MemberType, typename Comparator = std::less<MemberType>>
		static bool CompareLowerBound(const NoteDesc& s, const MemberType& v)
		{
			return Comparator()(s.timing, v);
		}

		template <typename MemberType, typename Comparator = std::less<MemberType>>
		static bool CompareUpperBound(const MemberType& v, const NoteDesc& s)
		{
			return Comparator()(v, s.timing);
		}
	};
	
	struct NoteDrawDesc
	{
	public:
		XMFLOAT4 color;
		double diameter;
		UINT textureID;
		UINT textureOverlayID;
	};
	static constexpr NoteDrawDesc defaultNoteDrawDesc{ MyColor4::Blank, 0 };

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

	Sprite laneSprite;
	Sprite judgeLineSprite;

public:
	double GetJudgePosition() const;
	void SetJudgePosition(double val);
private:
	double judgePosition = 0.0;

private:
	vector<NoteDesc> noteList; //weak ptr container
	void AddNoteDescFromNote(const MusicScore* score, const Note* const& targetNote);
	void ResetNoteVisible();

	set<size_t> targetNoteTypeList;
	void RemoveDuplicatedTargetKey(const MusicScore::NoteContainer& wholeNoteList);
	void CalculateTotalExpectedNotes(const MusicScore::NoteContainer& wholeNoteList);

	map<size_t, NoteDrawDesc> noteDrawDescMap;

	vector<NoteDesc>::iterator currentNote;

};