#pragma once
#include "framework.h"
#include "System/AlignMode.h"
#include "System/World/World3D.h"

/*
class SpriteDesc
{
public:
	SpriteDesc();
	~SpriteDesc();
	ID3D11ShaderResourceView* const GetTexture() const { return textureSRV; }
	void SetTexture(ID3D11ShaderResourceView* ptr) { textureSRV = ptr; }

	const World3D& GetWorld3dConst() const { return world3d; }
	World3D& GetWorld3d() { return world3d; }

	XMFLOAT4 Diffuse = { 1,1,1,1 };
	bool ColorMode = false;

private:
	World3D world3d;
	ID3D11ShaderResourceView* textureSRV = nullptr;

};
*/
struct SpriteInstanceData
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 uvworld;
	XMFLOAT4 Diffuse;
	UINT TextureID;

	struct LayoutDesc
	{
		static constexpr D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			VertexColorTexture::InputLayoutDesc::desc[0],
			VertexColorTexture::InputLayoutDesc::desc[1],
			VertexColorTexture::InputLayoutDesc::desc[2],
			{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{"UVWORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{"UVWORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{"UVWORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{"UVWORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{"DIFFUSE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 128,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{"TEXTUREID", 0, DXGI_FORMAT_R32_UINT, 1, 144,  D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		};
		static constexpr UINT Length = ARRAYSIZE(desc);
	};
};

class Sprite
{
public:
	Sprite();
	Sprite(float w, float h);
	Sprite(float x, float y, float w, float h);
	Sprite(float x, float y, float w, float h, const XMFLOAT4 diffuse, const bool colormode);
	~Sprite();

	void Render(ID3D11DeviceContext*, const Camera&, size_t srvCount = 1);
	//static void Render(ID3D11DeviceContext*, const Camera&, SpriteDesc&);
	static void RenderInstanced(ID3D11DeviceContext*, const Camera&, ID3D11Buffer*, size_t instanceOffset, size_t instanceCount, ID3D11ShaderResourceView* srv);
	void OnResize();

	void ChangeWidthToCurrentWidth(float w, float h);

	static void BulidBuffer(ID3D11Device*);
	static void BuildLayout(ID3D11Device*);

	void SetTexture(ID3D11ShaderResourceView* ptr) { textureSRV = ptr; }
	ID3D11ShaderResourceView* const GetTexture() const { return textureSRV; }

	const World3D& GetWorld3dConst() const { return world3d; }
	World3D& GetWorld3d() { return world3d; }

	void RepeatTexture(UINT imgWidth, UINT imgHeight, float imgRate = 1.0f);
	void RepeatTextureInExtraArea(UINT imgWidth, UINT imgHeight);

	XMFLOAT4 Diffuse;
	bool ColorMode = false;

private:
	void Init(float x, float y, float w, float h, const XMFLOAT4 diffuse, const bool colormode);

	World3D world3d;
	ID3D11ShaderResourceView* textureSRV; //weak reference, never delete this pointer

	void MakeCenterUV();

public:
	static ComPtr<ID3D11Buffer> mVB;
	static ComPtr<ID3D11Buffer> mIB;
	static ComPtr<ID3D11InputLayout> mInputLayout;
	static ComPtr<ID3D11InputLayout> mInsatncedInputLayout;

};

