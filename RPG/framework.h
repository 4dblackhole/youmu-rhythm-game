#pragma once

//Windows
#include <Windowsx.h>
#include <wrl.h>
using namespace Microsoft::WRL;

//#include "resource.h"

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
#include <unordered_map>
#include <memory.h>
#include <mutex>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <vector>
#include <tchar.h>
#include <thread>
#include <queue>

#include <iostream>
#include <iomanip>
using namespace std;


//Fmod
#include <fmod.hpp>
#include <fmod_errors.h>

//defines
#include "defines.h"
#include "Types.h"

//my headers
#include "Utility/Node.h"
#include "Utility/ShortCut.h"
#include "Utility/Math.h"
#include "Utility/RationalNumber.h"

#include "System/Keyboard.h"
#include "System/Camera.h"
#include "System/D2Ddevice.h"

#include "System/World/World2D.h"
#include "System/World/World3D.h"

#include "Effect/Effects/Effects.h"
#include "Effect/RenderStates.h"

#include "Object/Texture.h"
#include "Object/Vertex.h"

#include "Object/Shape2D/DwLayout2D.h"
#include "Object/Shape2D/Triangle2D.h"
#include "Object/Shape2D/Rectangle2D.h"

#include "MusicalObject/Music.h"
#include "MusicalObject/Note.h"
#include "MusicalObject/Pattern.h"
#include "MusicalObject/PrefixSum/MeasurePrefixSum.h"
#include "MusicalObject/PrefixSum/BpmTimingPrefixSum.h"

#include "Singleton/SceneManager.h"
#include "Singleton/FmodSystem.h"

#include "App/MainApp.h"
extern MainApp* App;

//all coordinates are based on these numbers.
constexpr UINT StandardWidth = 1280;
constexpr UINT StandardHeight = 720;

namespace MyColorF
{
	extern const D2D1::ColorF BlackHalfAlpha;
	extern const D2D1::ColorF WhiteHalfAlpha;
	extern const D2D1::ColorF GhostGreen;
	extern const D2D1::ColorF CherryPink;
}
namespace MyColor4
{
	constexpr DirectX::XMFLOAT4 Blank(0, 0, 0, 0);
	constexpr DirectX::XMFLOAT4 Black(0, 0, 0, 1);
	constexpr DirectX::XMFLOAT4 White(1, 1, 1, 1);
	constexpr DirectX::XMFLOAT4 MyRed(1, 56.0f/255.0f, 40.0f/255.0f, 1);
	constexpr DirectX::XMFLOAT4 MyBlue(64.0f/255.0f, 152.0f/255.0f, 1, 1);
	constexpr DirectX::XMFLOAT4 GhostGreen(48.0f / 255.0f, 224.0f / 255.0f, 104.0f / 255.0f, 1);
}

constexpr DirectX::XMFLOAT4 DefaultBgColor = MyColor4::Black;

extern const wstring SkinDir;
extern const wstring SongDir;
extern const wstring MusicDir;
extern const wstring PatternDir;
extern const wstring TextureDir;
extern const wstring SystemSoundDir;

constexpr LPCWSTR ExtMusic = L".ymm";
constexpr LPCWSTR ExtPattern = L".ymp";

constexpr XMFLOAT4X4 XmFloatT4X4Identity
{
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};

#include "Object/TextureManager.h"
