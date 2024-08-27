#pragma once
#include "framework.h"

struct MusicalPosition;
class Note;
class Measure;
class MusicBPM;

class MusicScore
{
private:
	struct MeasureNode;
public:
	MusicScore();
	~MusicScore();
	
	Note* FindFirstNote();
	RationalNumber<64> GetMeasureMusicalPosition(size_t pos); //first measure's position is 0

	using NoteList = map<MusicalPosition, Note>;

	//idx - length
	vector<Measure> measures;

	//time order
	set<MusicBPM> bpms;
	unordered_map<size_t, NoteList> notesPerTypeMap;

	double baseBpm = 120.0;
	chrono::microseconds offset{ 0 };

	void InitTree();
	void ReleaseTree();
	RationalNumber<64> GetMeasureLength(int idx);
	RationalNumber<64> GetMeasureLength(int listLeft, int listRight);

private:
	//segment tree of Measure
	struct MeasureNode
	{
		RationalNumber<64> totalLength{ 0, 1 };
		MeasureNode* pLeft = nullptr, *pRight = nullptr;
	};
	MeasureNode* measureSegTree = nullptr;
	MeasureNode* measureSegLazyTree = nullptr;
	
	//for debug
	vector<MeasureNode*> segTreeView;
	vector<MeasureNode*> segLazyTreeView;

	void InitTreeRC(MeasureNode* node, MeasureNode* lazyNode, int listLeft, int listRight);
	void ReleaseTreeRC(MeasureNode* node);

	RationalNumber<64> GetMeasureLengthRC(MeasureNode* node, MeasureNode* lazyNode, int searchLeft, int searchRight, int listLeft, int listRight);

	void UpdateLazyTree(MeasureNode* node, MeasureNode* lazyNode, int listLeft, int listRight);
	void UpdateMeasureRC(MeasureNode* node, MeasureNode* lazyNode, int listLeft, int listRight, RationalNumber<64> val);

	void RightRotate(MeasureNode* node);
};

class Measure
{
public:
	Measure() {};
	Measure(const RationalNumber<64>& q) :length(q) {}
	Measure(RationalNumber<64>&& q) noexcept :length(q) {}
	~Measure() {};

	RationalNumber<64> length;
};

struct MusicalPosition
{
public:
	bool operator<(const MusicalPosition& v) const;
	bool operator>(const MusicalPosition& v) const;
	bool operator<=(const MusicalPosition& v) const;
	bool operator>=(const MusicalPosition& v) const;
	bool operator==(const MusicalPosition& v) const;
	size_t measureIdx = 0;
	RationalNumber<64> position;
};

class MusicalObject
{
public:
	MusicalObject() {}
	MusicalObject(size_t idx, RationalNumber<64> pos) : mp({ idx, pos }) {}
	virtual ~MusicalObject() {};

	size_t GetMeasureIdx() const { return mp.measureIdx; }

	bool operator<(const MusicalObject&v) const;
	bool operator>(const MusicalObject&v) const;
	bool operator==(const MusicalObject& v) const;

	MusicalPosition mp;
};

class Note : public MusicalObject
{
public:
	Note() {}
	virtual ~Note() {}

	enum class DataOrder
	{
		Beat,
		Key,
		Action,
		Hitsound,
		ExtraData,
		MAX
	};

	UINT noteType = 0;
	UINT actionType = 0;
	wstring hitSound;
	wstring extraData;
};

class MusicBPM : public MusicalObject
{
public:
	MusicBPM(MusicalObject obj = {}, double b = 120.0) :MusicalObject(obj), bpm(b) {}
	virtual ~MusicBPM() {}

	double BPM() const { return bpm; }

protected:
	double bpm;
};

class MusicalEffect : public MusicalObject
{
public:
	MusicalEffect() {}
	virtual ~MusicalEffect() {}

protected:
	string type;
	string val;
};