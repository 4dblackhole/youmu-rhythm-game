#pragma once
#include "System/GameScene.h"
#include "Object/Sprite.h"
#include "framework.h"

#include "MusicalObject/Music.h"
#include "MusicalObject/Pattern.h"
#include "MusicalObject/Note.h"
#include "MusicalObject/Lane.h"
#include "MusicalObject/AccuracyRange.h"

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

	ID3D11Texture2D* pauseBG = nullptr;
	ID3D11RenderTargetView* pauseBgRTV = nullptr;
	ID3D11ShaderResourceView* pauseBgSRV = nullptr;

	Sprite prevSceneSprite;
	void InitPauseBackground();
	void ReleasePauseBackground();

//timing related
private:
	GameTimer timer;

	GameTimer rhythmTimer;
	chrono::microseconds musicTimeOffset{ 0 };
	chrono::microseconds firstNoteTiming{ 0 };
	double totalMusicTime = 0;
	
	template<typename DurationType>
	void UpdateMusicTimeOffset(DurationType waitTime);
	void UpdateTotalMusicTime();

	thread playMusicThread;
	bool playMusicThreadRunFlag = false;
	void StopPlayMusicThread();
	void PlayMusic();

	void StopThread();

//Game mode related
private:
	Lane testLane;
	void InitLanes();

	class TextureName
	{
	public:
		DECLARE_VARIABLE_STRING(LaneBackground);
		DECLARE_VARIABLE_STRING(note);
	};

	map<const string, Texture*> textureList;

	enum class SpriteTextureID
	{
		Note,
		NoteOverlay,
		MAX
	};

	void InitTextures();
	void ReleaseTextures();

	bool LoadTextureArray(map<const string, Texture*>&container, const string& keyStr, const vector<LPCWSTR>& fileList);
	bool LoadTextureArrayIndexed(map<const string, Texture*>&container, const string& keyStr, const wstring& fileName);

	bool LoadTextureArrayFromResource(map<const string, Texture*>& container, const string& keyStr, 
		const vector<ID3D11Texture2D*>& textureList);

	AccuracyRange accRange;
	map<size_t, FMOD::Sound*> defaultHitSoundList;

	//prefix sum for calculating timing
	MeasurePrefixSum measurePrefixSum;
	BpmTimingPrefixSum bpmTimingPrefixSum;
	void InitTimeSignaturePrefixSum();

public:
	//TODO: Change the owner of this function
	static chrono::microseconds GetNoteTimingPoint(const MeasurePrefixSum& measureSum, const BpmTimingPrefixSum& bpmSum, const Note& note);

private:
	Sprite laneSprite;
	Sprite noteSprite;
	Sprite noteOverlaySprite;
	bool noteInstancedBufferUpdateFlag;
	size_t instanceMaxSize = 1024;
	ComPtr<ID3D11Buffer> noteInstancedBuffer;
	
	void InitSprites();
	void InitInstancedBuffer();
	void UpdateInstancedBuffer();

	DwLayout2D currentTimeText;
	void InitCurrentTimeText();
	void UpdateCurrentTimeText();

	DwLayout2D loadingText;
	DwLayout2D loadingCompleteText;
	void InitLoadingText();

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

	void ParseBarLine(const wstring_view& lineStr, const RationalNumber<64>measureLength, size_t& measureIdx);
	void ParseMeasure(const wstring_view& lineStr, RationalNumber<64>& measureLength);
	bool ParseEffect(const wstring_view& lineStr, RationalNumber<64>& resultSignature, wstring& resultEffectStr);
	void ParseBPM(const wstring_view& str, const size_t measureIdx, const RationalNumber<64>& pos);
};

template<typename DurationType>
inline void PlayScene::UpdateMusicTimeOffset(DurationType waitTime)
{
	musicTimeOffset = waitTime;
}
