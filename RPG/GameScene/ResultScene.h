#pragma once
#include "System/GameScene.h"
#include "libraries.h"

class ResultScene : public GameScene
{
public:
	ResultScene();
	virtual ~ResultScene();

	virtual void BeginScene() final;
	virtual void OnResize(float newW, float newH) final;
	virtual void Update(float dt) final;
	virtual void Render(ID3D11DeviceContext* deviceContext, const Camera& cam) final;
	virtual void EndScene() final;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) final {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) final {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) final {}

	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) final {}
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) final {}

private:

	TextureManager textureManager;
	class TextureName
	{
	public:
		DECLARE_VARIABLE_STRING(fmodLogo);
	};

	GameTimer timer;

	DwLayout2D text;

};

