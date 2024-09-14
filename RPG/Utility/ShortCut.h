#pragma once
#include <D2D1.h>
#include <D3D11.h>
#include <iostream>
#include <tchar.h>
#include <string>
#include "Trace.h"
#include "RationalNumber.h"

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

	static std::wstring ReadUTF8File(const std::wstring& fileName);

	static void TraceTimingPoint();

	static float GetOrthoWidth(float w, float h);

	static D2D1_POINT_2F Resize2DtoStandardCS(float newW, float newH, float x, float y, float alignPosX = 0.0f);

	static void GetFileList(vector<wstring>& vList, const wstring& sPath, const wstring& ext, bool bAllDirectories = true);

	template <typename T>
	static BOOL FileExists(const T* szPath)
	{
		if constexpr (std::is_same_v<T, char>)
		{
			DWORD dwAttrib = GetFileAttributesA(szPath);
			return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
				!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
		}
		else if constexpr (std::is_same_v<T, wchar_t>)
		{
			DWORD dwAttrib = GetFileAttributesW(szPath);
			return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
				!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
		}
	}

	static bool WordSeparateA(const string_view& source, const string& separator, string* first, string* second);
	static bool WordSeparateW(const wstring_view& source, const wstring& separator, wstring* first, wstring* second);

	static void WordSeparateW(const wstring_view& source, const wstring& separator, vector<pair<size_t, size_t>>& idxList);

	static D3D11_TEXTURE2D_DESC GetDescFromSRV(ID3D11ShaderResourceView*& ptr);

	template <UINT32 Bits>
	static RationalNumber<Bits> StrToRationalNumber(const wstring_view& source);
	static D2D1::Matrix3x2F XmFloat4x4To3x2(const XMFLOAT4X4 m);
	static D2D1::Matrix3x2F WVP3Dto2D(const XMFLOAT4X4 m, float width, float height);

	template <typename T>
	struct ptrHeapLess {

		constexpr bool operator()(const T* _Left, const T* _Right) const
		{
			return (*_Left) > (*_Right);
		}
	};
};

template <typename T>
inline void SafeDelete(T*& ptr) { delete ptr; ptr = nullptr; }

template <typename T>
inline void SafeDeleteArr(T*& ptr) { delete[] ptr; ptr = nullptr; }

template <typename T>
std::enable_if_t<std::is_base_of<IUnknown, T>::value>
ReleaseCOM(T*& resource) {
	if (resource) {
		resource->Release();
		resource = nullptr;
	}
}

//#define ReleaseCOM(x) { if(x) { x->Release(); x = nullptr; } }

template <typename T, typename V>
T ReInterpret(V val)
{
	return *((T*)&val);
}

#define AlertBox(x) (MessageBox(NULL,(x),NULL,NULL))

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

template <UINT32 Bits>
inline RationalNumber<Bits> ShortCut::StrToRationalNumber(const wstring_view& source)
{
	wstring numerator, denominator;
	bool validCheck = WordSeparateW(source, L"/", &numerator, &denominator);
	assert(validCheck);

	typename RationalNumber<Bits>::SignedType nm;
	typename RationalNumber<Bits>::UnsignedType dnm;

	wstringstream wss;
	wss << numerator << L" " << denominator;
	wss >> nm >> dnm;

	assert(dnm != 0);
	//if (dnm == 0) return RationalNumber<Bits>(7, 22);
	return RationalNumber<Bits>(nm, dnm);
}
