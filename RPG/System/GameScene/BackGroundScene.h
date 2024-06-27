#pragma once
#include "System/GameScene.h"
#include "Object/Square.h"

class BackGroundScene : public GameScene
{
public:
	BackGroundScene();
	virtual ~BackGroundScene();

	virtual void Init() final;
	virtual void OnResize() final;
	virtual void Update(float dt) final;
	virtual void Render(ID3D11DeviceContext* deviceContext, const Camera& cam) final;
	virtual void Release() final;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

private:
	Square bg;
};

