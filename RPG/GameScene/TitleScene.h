#pragma once
#include "System/GameScene.h"
#include "Object/Sprite.h"
#include "Object/Shape2D/Triangle2D.h"
#include "Object/Shape2D/DwLayout2D.h"

class TitleScene : public GameScene
{
public:
	TitleScene();
	virtual ~TitleScene();

	virtual void BeginScene() final;
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
	Sprite tooltipBG;
	Sprite titleLogoImg;
	Texture titleSRV;

	enum class SelectKey
	{
		GameStart,
		Options,
		Exit,
		MAX
	};
	SelectKey selectKey = SelectKey::GameStart;
	Triangle2D* keySelectTriangle{};
	void ChangeTrianglePos(SelectKey key);


	enum class LayoutKey
	{
		GameStart,
		Options,
		Exit,
		Tooltip,
		MAX
	};
	vector<DwLayout2D> layoutList;
	void InitLayout();

	FMOD::Sound* bgm{};
	FMOD::Channel* bgmChannel{};

};

