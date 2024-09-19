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