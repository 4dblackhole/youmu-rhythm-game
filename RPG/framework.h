#pragma once

//Windows
#include <Windowsx.h>
#include <wrl.h>
using namespace Microsoft::WRL;

//DirectX
#include <D2D1_1.h>
#include <dwrite.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <effects.h>
#include <dxgidebug.h>
#include <DxErr.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <d3dx11effect.h>

using namespace DirectX;


//STL
#include <assert.h>
#include <chrono>
#include <fstream>
#include <functional>
#include <malloc.h>
#include <map>
#include <memory.h>
#include <mutex>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <tchar.h>
#include <thread>

#include <iostream>
#include <iomanip>
using namespace std;


//Fmod
#include <fmod.hpp>

//my headers
#include "Utility/ShortCut.h"
#include "Utility/Math.h"
#include "Utility/RationalNumber.h"

#include "System/Keyboard.h"
#include "System/Camera.h"
#include "System/D2Ddevice.h"

#include "Effect/Effects.h"
#include "Effect/RenderStates.h"

#include "Singleton/TextureManager.h"
#include "Singleton/SceneManager.h"

#include "Object/DwLayout.h"



#include "App/D3DApp.h"
extern const D3DApp* App;

//all coordinates are based on these numbers.
constexpr UINT StandardWidth = 1280;
constexpr UINT StandardHeight = 720;

namespace MyColorF
{
	extern const D2D1::ColorF GhostGreen;
}
namespace MyColor4
{
	constexpr DirectX::XMFLOAT4 Black(0, 0, 0, 1);
	constexpr DirectX::XMFLOAT4 White(1, 1, 1, 1);
}

constexpr LPCWSTR SongsDir = L"Songs/";