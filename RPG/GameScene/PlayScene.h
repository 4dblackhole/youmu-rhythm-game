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

private:

	enum class Status
	{
		Load,
		Start,
		Resume,
		Pause,
		End
	};

	Status sceneStatus = Status::Load;
	Status prevSceneStatus = Status::Load;

	void ChangeStatus(Status);
	void ChangeStatusLoad();
	void ChangeStatusStart();
	void ChangeStatusReStart();
	void ChangeStatusResume();
	void ChangeStatusPause();
	void ChangeStatusEnd();

	void UpdateOnLoad(float dt);
	void UpdateOnStart(float dt);
	void UpdateOnResume(float dt);
	void UpdateOnPause(float dt);
	void UpdateOnEnd(float dt);

	void RenderStatus(Status s, ID3D11DeviceContext* deviceContext, const Camera& cam);
	void RenderOnLoad(ID3D11DeviceContext* deviceContext, const Camera& cam);
	void RenderOnStart(ID3D11DeviceContext* deviceContext, const Camera& cam);
	void RenderOnResume(ID3D11DeviceContext* deviceContext, const Camera& cam);
	void RenderOnPause(ID3D11DeviceContext* deviceContext, const Camera& cam);
	void RenderOnEnd(ID3D11DeviceContext* deviceContext, const Camera& cam);

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

private:
	GameTimer timer;
	GameTimer rhythmTimer;
	Sprite transparentBlackBG;

	Music* music; //weak ptr
	Pattern* pattern; //weak ptr

	Lane testLane;
	void InitLanes();

	AccuracyRange accRange;

	static constexpr chrono::milliseconds ReadyTime{ 1000 };

private:
	DwLayout2D currentTimeText;
	void InitCurrentTimeText();
	void UpdateCurrentTimeText();

	DwLayout2D loadingText;
	DwLayout2D loadingCompleteText;
	void InitLoadingText();

	bool musicScoreLoadFlag = false;
	MusicScore* musicScore = nullptr; //note container
	void LoadMusicScore();
	void LoadMusicScoreComplete();

	void LoadTimeSignature(const wstring_view& content);
	void LoadPattern(const wstring_view& content);

	void ParseBarLine(const wstring_view& lineStr, const RationalNumber<64>measureLength, size_t& measureIdx);
	void ParseMeasure(const wstring_view& lineStr, RationalNumber<64>& measureLength);
	bool ParseEffect(const wstring_view& lineStr, RationalNumber<64>& resultSignature, wstring& resultEffectStr);
	void ParseBPM(const wstring_view& str, const size_t measureIdx, const RationalNumber<64>& pos);
};