#pragma once
#include "D3DApp.h"
#include "Object/Sprite.h"
#include "Object/DwLayout.h"

class MainApp : public D3DApp
{
public:
	MainApp(HINSTANCE hInstance);
	~MainApp();

	bool Init() final;
	void OnResize() final;
	void UpdateScene(float dt) final;
	void DrawScene() final;

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
	DwLayout *fpsLayout;

	ID2D1Bitmap* bitmap;

};