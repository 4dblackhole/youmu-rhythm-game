#pragma once
#include "framework.h"

class GameScene
{
public:
	virtual void Init() = 0;
	virtual void OnResize() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render(ID3D11DeviceContext* deviceContext, const Camera& cam) = 0;
	virtual void Release() = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) = 0;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) = 0;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) = 0;
};

