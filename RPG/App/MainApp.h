#pragma once
#include "framework.h"
#include "D3DApp.h"
#include "Object/Sprite.h"
#include "Object/Shape2D/DwLayout2D.h"

class MainApp : public D3DApp
{
public:
	MainApp(HINSTANCE hInstance);
	~MainApp();

	bool Init() final;
	void OnResize() final;
	void UpdateScene(float dt) final;
	void DrawScene() final;

	const float RateY() const { return rateY; }
	void ChangeBgColor(const XMFLOAT4 c) { bgColor = c; }

	virtual void OnMouseDown(WPARAM btnState, int x, int y) final;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) final;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) final;
	virtual void OnMouseWheel(WPARAM btnState, int x, int y) final;
	
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) final;
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) final;

private:
	void BuildBuffer();
	void BuildLayout();

	void InitGameScenes();

	bool drawFPSCounterFlag;
	void UpdateFPS();

private:
	Camera mCamera;
	GameTimer fpsTimer;
	enum class LayoutKey
	{
		Frame,
		MAX
	};
	DwLayout2D *fpsLayout;

	float rateY = 1.0f;

	XMFLOAT4 bgColor;

	//DrawWorld, UpdateFlag
	pair<D2D1::Matrix3x2F, bool> mDrawWorld2DArr[(int)AlignModeX::MAX];
	pair<XMFLOAT4X4, bool> mDrawWorld3DArr[(int)AlignModeY::MAX][(int)AlignModeX::MAX];
	void InitDrawWorld();

	void UpdateDrawWorld2D(AlignModeX x);
	void UpdateDrawWorld3D(AlignModeX x, AlignModeY y);
	void NotifyDrawWorldResize();

public:
	const D2D1::Matrix3x2F& GetDrawWorld2D(const AlignModeX x = AlignModeX::Left);
	const XMFLOAT4X4& GetDrawWorld3D(const AlignModeX x = AlignModeX::Mid, const AlignModeY y = AlignModeY::Mid);

private:
};