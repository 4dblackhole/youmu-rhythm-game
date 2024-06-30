#pragma once
#include "D3DApp.h"
#include "Object/Sprite.h"

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
	
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) final;
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) final;

private:
	void BuildBuffer();
	void BuildLayout();

	void InitGameScenes();

private:
	Camera mCamera;


};