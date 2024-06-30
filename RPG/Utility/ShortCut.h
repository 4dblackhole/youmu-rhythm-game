﻿#pragma once
#include <D3D11.h>
#include <iostream>
#include <tchar.h>
#include <string>
#include "Trace.h"

namespace std
{
	using tstring = std::basic_string<TCHAR>;
	using tistringstream = std::basic_istringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>>;
	using tostringstream = std::basic_ostringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>>;
}

class ShortCut
{
public:

	static std::wstring UTF8ToWstring(const std::string& buf);
	static std::string WstringToUTF8(const std::wstring& buf);

	static void TraceTimingPoint();

	static float GetOrthoWidth(float w, float h);
};

template <typename T>
inline void SafeDelete(T*& ptr) { delete ptr; ptr = nullptr; }

template <typename T>
inline void SafeDeleteArr(T*& ptr) { delete[] ptr; ptr = nullptr; }

#define ReleaseCOM(x) { if(x) { x->Release(); x = nullptr; } }


#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, _T(#x), true); \
		}                                                      \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

namespace Colors
{
	constexpr float White[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	constexpr float Black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	constexpr float Red[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	constexpr float Green[4] = {0.0f, 1.0f, 0.0f, 1.0f};
	constexpr float Blue[4] = {0.0f, 0.0f, 1.0f, 1.0f};
	constexpr float Yellow[4] = {1.0f, 1.0f, 0.0f, 1.0f};
	constexpr float Cyan[4] = {0.0f, 1.0f, 1.0f, 1.0f};
	constexpr float Magenta[4] = {1.0f, 0.0f, 1.0f, 1.0f};
	constexpr float Silver[4] = {0.75f, 0.75f, 0.75f, 1.0f};
	constexpr float LightSteelBlue[4] = {0.69f, 0.77f, 0.87f, 1.0f};
}