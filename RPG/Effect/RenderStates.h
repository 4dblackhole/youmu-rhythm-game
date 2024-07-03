#pragma once
#include <Windowsx.h>
#include <wrl.h>

#include <D3D11.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <effects.h>
#include <dxgidebug.h>
#include <DxErr.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <d3dx11effect.h>

class RenderStates
{
public:
	static void Init(ID3D11Device* device);
	static void Release() {};

	// Rasterizer states
	static Microsoft::WRL::ComPtr<ID3D11RasterizerState> WireframeRS;
	static Microsoft::WRL::ComPtr<ID3D11RasterizerState> NoCullRS;
	static Microsoft::WRL::ComPtr<ID3D11RasterizerState> CullClockwiseRS;

	static Microsoft::WRL::ComPtr<ID3D11BlendState> TransparentBS;

	// Depth/stencil states
	static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> NoDepthDSS;

};

