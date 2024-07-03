#pragma once
#include <windowsx.h>
#include <wrl.h>

#include <D2D1_1.h>
#include <dwrite.h>
#include <dxgi.h>

#include <string>
#include <map>
#include "Utility/ShortCut.h"
#include "Singleton/SingletonBase.h"

class DirectWrite : public SingletonBase<DirectWrite>
{
public:
	DirectWrite();
	~DirectWrite();

	void CreateD2DDWFactory();
	void ResetBackBuffer(IDXGISwapChain* swapChain);
	void ResetBackBuffer_Release();

	void BeginDraw() { d2Rtg->BeginDraw(); }
	void EndDraw() { d2Rtg->EndDraw(); }

	ID2D1SolidColorBrush*& GetSolidBrush(const D2D1::ColorF color);
	IDWriteTextFormat*& GetFont(const std::string name);

	IDWriteFactory* GetDwFactory() const { return dwFactory.Get(); }

private:
	void InitFonts();

private:
	//D2 Devices
	Microsoft::WRL::ComPtr<IDWriteFactory> dwFactory;
	Microsoft::WRL::ComPtr<ID2D1Factory> d2dFactory;
	ID2D1RenderTarget* d2Rtg = nullptr;
	IDXGISurface* pBackBuffer = nullptr;

	//Brush
private:
	struct CompareD2D1ColorF {
		bool operator()(const D2D1::ColorF& a, const D2D1::ColorF& b) const 
		{
			uint16_t aVal = ReInterpret<uint16_t>(a);
			uint16_t bVal = ReInterpret<uint16_t>(b);
			return aVal < bVal;
		}
	};
	std::map<const D2D1::ColorF, ID2D1SolidColorBrush*, CompareD2D1ColorF> brushList;

	//Font
private:
	std::map<const std::string, IDWriteTextFormat*> fontList;
public:
	class FontName
	{
	public:
		static constexpr LPCSTR DefaultFont = "DefaultFont";
	};

public:
	void DrawTextLayout(D2D1_POINT_2F origin,
		_In_ IDWriteTextLayout* textLayout,
		_In_ ID2D1Brush* defaultFillBrush,
		D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE)
	{
		d2Rtg->DrawTextLayout(origin, textLayout, defaultFillBrush, options);
	}
	void DrawText(_In_reads_(stringLength) CONST TCHAR* string,
		UINT32 stringLength,
		_In_ IDWriteTextFormat* textFormat,
		CONST D2D1_RECT_F& layoutRect,
		_In_ ID2D1Brush* defaultFillBrush,
		D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE,
		DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL)
	{
		d2Rtg->DrawText(string, stringLength, textFormat, layoutRect, defaultFillBrush, options, measuringMode);
	}
};


#define DIRECTWRITE DirectWrite::GetInstance()