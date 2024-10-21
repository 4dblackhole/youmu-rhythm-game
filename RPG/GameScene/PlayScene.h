#pragma once
#include "System/GameScene.h"
#include "Object/Sprite.h"
#include "framework.h"

#include "MusicalObject/Music.h"
#include "MusicalObject/Pattern.h"
#include "MusicalObject/Note.h"
#include "MusicalObject/Lane.h"
#include "MusicalObject/AccuracyRange.h"
#include "MusicalObject/ScorePercentage.h"

class PlayScene : public GameScene
{
public:
	PlayScene(Music* m, Pattern* p);
	virtual ~PlayScene();

	virtual void BeginScene() final;
	virtual void OnResize(float newW, float newH) final;
	virtual void Update(float dt) final;
	virtual void Render(ID3D11DeviceContext* deviceContext, const Camera& cam) final;
	virtual void EndScene() final;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) final {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) final {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) final {}

	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) final {};
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) final {};

//state machine related
private:

	enum class Status
	{
		Load,
		Start,
		Pause,
		End
	};

	Status sceneStatus = Status::Load;
	Status prevSceneStatus = Status::Load;

	void ChangeStatusLoad();
	void ExitStatusLoad();
	void UpdateOnLoad(float dt);
	void RenderOnLoad(ID3D11DeviceContext* deviceContext, const Camera& cam);

	void ChangeStatusReStart();
	void ChangeStatusStart();
	void ExitStatusStart();
	void UpdateOnStart(float dt);
	void RenderOnStart(ID3D11DeviceContext* deviceContext, const Camera& cam);
	
	void ChangeStatusPause();
	void UpdateOnPause(float dt);
	void RenderOnPause(ID3D11DeviceContext* deviceContext, const Camera& cam);
	
	void ChangeStatusEnd();
	void UpdateOnEnd(float dt);
	void RenderOnEnd(ID3D11DeviceContext* deviceContext, const Camera& cam);

	void StopThread();

	void ChangeStatus(Status);
	void ExitStatus(Status);
	void RenderStatus(Status s, ID3D11DeviceContext* deviceContext, const Camera& cam);
	
//pause related
private:
	enum class PauseOption
	{
		Resume,
		Restart,
		Quit,
		MAX
	};
	int pauseOptionKey = (int)PauseOption::Resume;
	void ChangePauseOptionKey(int val);
	void InitPauseOptionLayoutList();

	DwLayout2D PauseOptLayoutList[(int)PauseOption::MAX];
	static constexpr LPCWSTR pauseOptTxtMap[(int)PauseOption::MAX]
	{ L"Resume",L"Restart",L"Quit" };

	Triangle2D pauseOptSelectTriangle{};

	ID3D11RenderTargetView* pauseBgRTV = nullptr;
	Texture pauseBgTexture;

	Sprite prevSceneSprite;
	void InitPauseBackground();
	void ReleasePauseBackground();

//restart related
private:
	void ReSetting();

//timing related
private:
	GameTimer threadTimer;

	GameTimer rhythmTimer;
	chrono::microseconds musicTimeOffset{ 0 };
	chrono::microseconds firstNoteTiming{ 0 };
	MilliDouble totalMusicTime{ 0.0 };
	
	void UpdateTotalMusicTime();

	thread playMusicThread;
	bool playMusicThreadRunFlag = false;
	void StopPlayMusicThread();
	void PlayMusic();

	const AccuracyRange accRange;

//Score and Percentage related
private:
	ScorePercentage scorePercent;

//Game mode related
private:
	Lane testLane;
	void InitTaikoModeLanes();

public:
	//TODO: move these enums to PlayMode class
	enum class TaikoNoteType
	{
		Don = 1,
		Kat,
		BigDon,
		BigKat,
		Roll=11,
		BigRoll,
		Balloon,
		MAX
	};

	enum class HitCount
	{
		NoCount,
		Note,
		BigNote,
		MAX
	};

private:
	map<wstring, vector<::byte>> keyNoteTypeMap;
	DECLARE_VARIABLE_WSTRING(LeftD);
	DECLARE_VARIABLE_WSTRING(RightD);
	DECLARE_VARIABLE_WSTRING(LeftK);
	DECLARE_VARIABLE_WSTRING(RightK);
	void InitTaikoModeKeyNoteTypeMap();
	void ReleaseTaikoModeKeyNoteTypeMap();
	void NoteUpdateTaikoMode(const MilliDouble& refTime);
	void NoteProcessTaikoMode(const MilliDouble& refTime, const std::span<const UINT>& targetTypeList);
	bool CheckNoteType(const std::span<const UINT>& targetTypeList);
	void MoveTargetNote(const MilliDouble refTime, const AccuracyRange::RangeName& judgepriority);

	enum class DefaultHitSound
	{
		Don,
		Kat,
		BigDon,
		BigKat,
		MAX
	};
	map<size_t, FMOD::Sound*> defaultTaikoModeHitSoundList;
	void InitTaikoModeHitSounds();
	void ReleaseTaikoModeHitSounds();
	void PlayTaikoModeDonSound(const MilliDouble& refTime);
	void PlayTaikoModeKatSound(const MilliDouble& refTime);
	bool CheckNoteTypeForHitSound(const MilliDouble& refTime, UINT targetType, int targetHitCount);

	class TextureName
	{
	public:
		DECLARE_VARIABLE_STRING(LaneBackground);
		DECLARE_VARIABLE_STRING(LaneLight);
		DECLARE_VARIABLE_STRING(JudgeLine);
		DECLARE_VARIABLE_STRING(MeasureLine);
		DECLARE_VARIABLE_STRING(note);
	};

	TextureManager textureList;

	//Texture2DArray ID of Texture "note"
	enum class NoteTextureArrID
	{
		Note,
		BigNote,
		NoteOverlay,
		MAX
	};

	enum class NoteTextureInstanceID
	{
		Note, //can change color
		NoteOverlay, //can't change color
		MAX
	};

	void InitTaikoModeTextures();
	void ReleaseTaikoModeTextures();

	enum class ScrollSpeedMode
	{
		RhythmConstant,
		BPMconstant,
		MAX
	};
	ScrollSpeedMode scrollSpeedMode = ScrollSpeedMode::RhythmConstant;

private:
	void InitTaikoModeSprites();

	void InitInstancedBuffer(ID3D11Buffer*&, UINT);
	int noteInstanceCount = 0;
	ID3D11Buffer* noteInstancedBuffer;
	void UpdateInstancedBuffer_TaikoModeNote(ID3D11Buffer* instBuffer, const MilliDouble refTime, Lane& lane);
	void UpdateInstancedBuffer_TaikoModeNote_Internal(const MilliDouble refTime, Lane& lane, SpriteInstanceData*& dataView);

	int measureLineInstanceCount = 0;
	ID3D11Buffer* measureLineInstancedBuffer;
	void UpdateInstancedBuffer_MeasureLine(ID3D11Buffer* instBuffer, const vector<Measure>& measureList, const MilliDouble refTime, Lane& lane);
	void UpdateInstancedBuffer_MeasureLine_Internal(const MilliDouble refTime, const vector<Measure>& measureList, Lane& lane, SpriteInstanceData*& dataView);

	DwLayout2D currentTimeText;
	void InitCurrentTimeText();
	void UpdateCurrentTimeText();

	DwLayout2D loadingText;
	DwLayout2D loadingCompleteText;
	void InitLoadingText();

//debug related
private:
	MilliDouble debugMs{ 0 };
	DwLayout2D debugText;
	chrono::milliseconds differenceFromTime{};
	void InitDebugText();
	void UpdateDebugText();

//music score related
private:
	Music* music; //weak ptr
	Pattern* pattern; //weak ptr

	bool musicScoreLoadFlag = false;
	MusicScore* musicScore = nullptr; //note container

	thread loadMusicScoreThread;
	bool loadMusicScoreThreadFlag = false;
	void StopLoadMusicScoreThread();
	void LoadMusicScore();
	void LoadMusicScoreComplete();

	void LoadTimeSignature(const wstring_view& content);
	void LoadPattern(const wstring_view& content);
	bool GetNextLineIdxPair(const wstring_view& content, const wstring_view& endStr, size_t& startIdx, size_t& endIdx);

	void ParseBarLine(const wstring_view& lineStr, const Measure& measureRef, size_t& measureIdx);
	void ParseMeasure(const wstring_view& lineStr, RationalNumber<64>& measureLength);
	bool ParseEffect(const wstring_view& lineStr, RationalNumber<64>& resultSignature, wstring& resultEffectStr);
	void ParseBPM(const wstring_view& str, const size_t measureIdx, const RationalNumber<64>& pos);
};