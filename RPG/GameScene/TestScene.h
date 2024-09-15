#pragma once
#include "System/GameScene.h"
#include "Object/Sprite.h"

struct InstancedData
{
	XMFLOAT4X4 World;
};

class TestScene : public GameScene
{
public:
	TestScene();
	virtual ~TestScene();

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
	TextureManager textureManager;
	class TextureName
	{
	public:
		DECLARE_VARIABLE_STRING(test);
		DECLARE_VARIABLE_STRING(myon);
	};

	Sprite sq;
	Sprite sq2;
	Sprite whiteSquare;

	static constexpr size_t instanceSize = 50;
	vector<InstancedData> instances;
	void InitInstanceData();


	ID3D11Buffer* mVB = nullptr;
	ID3D11Buffer* mIB = nullptr;
	ID3D11InputLayout* mLayout = nullptr;
	ID3D11Buffer* mInstancedBuffer = nullptr;
	ID3D11InputLayout* mInstancedLayout = nullptr;

	void BuildBuffer();
	void BuildLayout();
	void BuildInstancedBuffer();
	void BuildInstancedLayout();

	void WhiteSquareLeftRightMove(float dt);

};

