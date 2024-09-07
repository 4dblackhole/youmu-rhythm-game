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

class D2Ddevice : public SingletonBase<D2Ddevice>
{
public:
	D2Ddevice();
	~D2Ddevice();

	void CreateD2DDWFactory();
	void ResetBackBufferFromSwapChain(IDXGISwapChain* swapChain);
	void ResetBackBuffer(ID3D11Texture2D* buffer);
	void ReleaseBackBuffer();

	void BeginDraw() noexcept { d2Rtg->BeginDraw(); }
	void EndDraw() noexcept { d2Rtg->EndDraw(); }

	ID2D1SolidColorBrush*& GetSolidBrush(const D2D1::ColorF color);
	IDWriteTextFormat*& GetFont(const std::string name);

	ID2D1Factory* GetD2DFactory() const { return d2dFactory.Get(); }
	IDWriteFactory* GetDwFactory() const { return dwFactory.Get(); }
	ID2D1RenderTarget* GetRenderTarget() const { return d2Rtg; }
private:
	void InitFonts();

private:
	//D2 Devices
	Microsoft::WRL::ComPtr<IDWriteFactory> dwFactory;
	Microsoft::WRL::ComPtr<ID2D1Factory> d2dFactory;
	ID2D1RenderTarget* d2Rtg = nullptr;
	IDXGISurface* pDxgiSurface = nullptr;

//Brush
private:
	struct CompareD2D1ColorF {
		bool operator()(const D2D1::ColorF& a, const D2D1::ColorF& b) const
		{
			return std::tie(a.r, a.g, a.b, a.a) < std::tie(b.r, b.g, b.b, b.a);
		}
	};
	std::map<const D2D1::ColorF, ID2D1SolidColorBrush*, CompareD2D1ColorF> brushList;

	bool AddSolidBrush(const D2D1::ColorF color);

//Font
private:
	std::map<const std::string, IDWriteTextFormat*> fontList;
public:
	class FontName
	{
	public:
		static constexpr LPCSTR DefaultFont = "DefaultFont";
	};

	static constexpr float DefaultFontSize = 15.0f;
public:
	void DrawTextLayout(D2D1_POINT_2F origin,
		_In_ IDWriteTextLayout* textLayout,
		_In_ ID2D1Brush* defaultFillBrush,
		D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE) noexcept
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


#define D2D D2Ddevice::GetInstance()