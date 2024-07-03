#pragma once
#include "System/GameScene.h"
#include "Object/Sprite.h"

class TestScene : public GameScene
{
public:
	TestScene();
	virtual ~TestScene();

	virtual void BeginScene(float newW, float newH) final;
	virtual void OnResize(float newW, float newH) final;
	virtual void Update(float dt) final;
	virtual void Render(ID3D11DeviceContext* deviceContext, const Camera& cam) final;
	virtual void EndScene() final;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) final {}
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) final {}

private:
	Sprite sq;
	Sprite sq2;

};

