#pragma once
#include "framework.h"

class GameScene
{
public:
	virtual ~GameScene() {}

	virtual void BeginScene() = 0;
	virtual void OnResize(float newW, float newH) = 0;
	virtual void Update(float dt) = 0;
	virtual void Render(ID3D11DeviceContext* deviceContext, const Camera& cam) = 0;
	virtual void EndScene() = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) = 0;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) = 0;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) = 0;

	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) = 0;
};

