#pragma once
#include "framework.h"

__declspec(align(16)) struct Vertex
{
public:
	Vertex();
	Vertex(const Vertex&);
	Vertex(Vertex&&) noexcept;
	Vertex& operator=(const Vertex&);
	Vertex& operator=(Vertex&&) noexcept;

	Vertex(const XMFLOAT3&);
	Vertex(XMFLOAT3&&) noexcept;
	Vertex& operator=(const XMFLOAT3&);
	Vertex& operator=(XMFLOAT3&&) noexcept;

	XMFLOAT3 Pos;

	struct InputLayoutDesc
	{
		static constexpr UINT Length = 1;
		static constexpr D3D11_INPUT_ELEMENT_DESC desc[Length] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	};
};

__declspec(align(16)) struct VertexColor
{
public:
	VertexColor();
	VertexColor(const VertexColor&);
	VertexColor(VertexColor&&) noexcept;
	VertexColor& operator=(const VertexColor&);
	VertexColor& operator=(VertexColor&&) noexcept;

	VertexColor(const XMFLOAT3&, const XMFLOAT4&);
	VertexColor(XMFLOAT3&&, XMFLOAT4&&) noexcept;

	XMFLOAT3 Pos;
	XMFLOAT4 Color;

	struct InputLayoutDesc
	{
		static constexpr UINT Length = 2;
		static constexpr D3D11_INPUT_ELEMENT_DESC desc[Length] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};
};

__declspec(align(16)) struct VertexColorTexture
{
public:
	VertexColorTexture();
	VertexColorTexture(const VertexColorTexture&);
	VertexColorTexture(VertexColorTexture&&) noexcept;
	VertexColorTexture& operator=(const VertexColorTexture&);
	VertexColorTexture& operator=(VertexColorTexture&&) noexcept;

	VertexColorTexture(const XMFLOAT3&, const XMFLOAT4&, const XMFLOAT2&);
	VertexColorTexture(XMFLOAT3&&, XMFLOAT4&&, XMFLOAT2&&) noexcept;

	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT2 Uv;
	float Padding[2]{};

	struct InputLayoutDesc
	{
		static constexpr UINT Length = 3;
		static constexpr D3D11_INPUT_ELEMENT_DESC desc[Length] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(VertexColorTexture::Pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(VertexColorTexture::Pos) + sizeof(VertexColorTexture::Color), D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};
};
