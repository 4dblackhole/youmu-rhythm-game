#pragma once
#include "System/GameScene.h"
#include "Object/Sprite.h"
#include "framework.h"

class LogoScene : public GameScene
{
public:
	LogoScene();
	virtual ~LogoScene();

	virtual void BeginScene(float newW, float newH) final;
	virtual void OnResize(float newW, float newH) final;
	virtual void Update(float dt) final;
	virtual void Render(ID3D11DeviceContext* deviceContext, const Camera& cam) final;
	virtual void EndScene() final;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) final {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) final {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) final {}

	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) final;
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) final;
private:
	void Init();
	void Release();

	Sprite fmodLogo;
	Sprite background;

	GameTimer bgTimer;

	enum BGState
	{
		Normal,
		Fade,
		End
	};
	BGState state = Normal;

	bool keyState[MAX_INPUT_KEY];
};

