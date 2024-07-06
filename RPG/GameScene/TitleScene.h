#pragma once
#include "System/GameScene.h"
#include "Object/Sprite.h"
#include "Object/DwLayout.h"
#include "Object/Shape2D/Triangle2D.h"

class TitleScene : public GameScene
{
public:
	TitleScene();
	virtual ~TitleScene();

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
	Sprite background;
	Sprite titleLogoImg;
	ID3D11ShaderResourceView* titleSrv = nullptr;

	enum LayoutKey
	{
		GameStart,
		Options,
		Exit,
		MAX
	};
	LayoutKey currentKey = GameStart;
	Triangle2D* keySelectTriangle;

	vector<DwLayout> layoutList;
	void InitLayout();


};

