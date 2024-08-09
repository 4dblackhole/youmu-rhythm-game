#pragma once
#include "System/GameScene.h"
#include "Object/Sprite.h"
#include "framework.h"

class PlayScene : public GameScene
{
public:
	PlayScene();
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
		Ready,
		Start,
		Resume,
		Pause,
		End
	};

	Status sceneStatus = Status::Ready;
	
	void ChangeStatusReady();
	void ChangeStatusStart();
	void ChangeStatusResume();
	void ChangeStatusPause();
	void ChangeStatusEnd();

	void UpdateOnReady(float dt);
	void UpdateOnStart(float dt);
	void UpdateOnResume(float dt);
	void UpdateOnPause(float dt);
	void UpdateOnEnd(float dt);

	void RenderOnPause(ID3D11DeviceContext* deviceContext, const Camera& cam);

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

	GameTimer resumeTimer;

	Sprite transparentBlackBG;
	Sprite youmu;

};