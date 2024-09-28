#pragma once
#include "InstanceBuffer.h"
#include "Object/Vertex.h"

template <typename RefDataSet>
class SpriteInstanceBuffer : public InstanceBuffer
{
public:
	__declspec(align(16)) struct InstanceData
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

public:
	SpriteInstanceBuffer();
	virtual ~SpriteInstanceBuffer();	

	void Update(const RefDataSet& refData);

private:
	virtual UINT GetBufferSizeFromCount(UINT count) final;
	virtual void UpdateInternal(const RefDataSet& refData, InstanceData*& dataView) = 0;

};