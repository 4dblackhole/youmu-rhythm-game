#pragma once
#include "framework.h"

struct Vertex
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

struct VertexColor : public Vertex
{
public:
	VertexColor();
	VertexColor(const VertexColor&);
	VertexColor(VertexColor&&) noexcept;
	VertexColor& operator=(const VertexColor&);
	VertexColor& operator=(VertexColor&&) noexcept;

	VertexColor(const XMFLOAT3&, const XMFLOAT4&);
	VertexColor(XMFLOAT3&&, XMFLOAT4&&) noexcept;

	XMFLOAT4 Color;

	struct InputLayoutDesc
	{
		static constexpr UINT Length = 2;
		static constexpr D3D11_INPUT_ELEMENT_DESC desc[Length] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(Vertex::Pos), D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};
};


struct VertexTexture : public Vertex
{
public:
	VertexTexture();
	VertexTexture(const VertexTexture&);
	VertexTexture(VertexTexture&&) noexcept;
	VertexTexture& operator=(const VertexTexture&);
	VertexTexture& operator=(VertexTexture&&) noexcept;

	VertexTexture(const XMFLOAT3&, const XMFLOAT2&);
	VertexTexture(XMFLOAT3&&, XMFLOAT2&&) noexcept;

	XMFLOAT2 Uv;
	float Padding[2]{};

	struct InputLayoutDesc
	{
		static constexpr UINT Length = 2;
		static constexpr D3D11_INPUT_ELEMENT_DESC desc[Length] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(Vertex::Pos), D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

	};

};
