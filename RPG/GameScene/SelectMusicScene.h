#pragma once
#include "libraries.h"
#include "System/GameScene.h"
#include "Object/Sprite.h"
#include "Object/Shape2D/Rectangle2D.h"
#include "Object/Shape2D/DwLayout2D.h"
#include "Object/MusicListScroll.h"

class SelectMusicScene : public GameScene
{
public:
	SelectMusicScene();
	virtual ~SelectMusicScene() final;

	virtual void BeginScene() final;
	virtual void OnResize(float newW, float newH) final;
	virtual void Update(float dt) final;
	virtual void Render(ID3D11DeviceContext* deviceContext, const Camera& cam) final;
	virtual void EndScene() final;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) final;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) final;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) final;
	virtual void OnMouseWheel(WPARAM wState, int x, int y) final;

	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) final {}
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) final {}

private:
	Sprite background;
	MusicListScroll musicScroll;

	DwLayout2D tooltipLayout;
	
};