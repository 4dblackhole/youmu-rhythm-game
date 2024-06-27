#pragma once
#include "framework.h"

class RenderStates
{
public:
	static void Init(ID3D11Device* device);
	static void Release() {};

	// Rasterizer states
	static ComPtr<ID3D11RasterizerState> WireframeRS;
	static ComPtr<ID3D11RasterizerState> NoCullRS;
	static ComPtr<ID3D11RasterizerState> CullClockwiseRS;

	static ComPtr<ID3D11BlendState> TransparentBS;

	// Depth/stencil states
	static ComPtr<ID3D11DepthStencilState> NoDepthDSS;

};

