#include "framework.h"
#include "D2Ddevice.h"

D2Ddevice::D2Ddevice()
{
	CreateD2DDWFactory();
	InitFonts();
}

D2Ddevice::~D2Ddevice()
{
	for (pair<const D2D1::ColorF, ID2D1SolidColorBrush*>& it : brushList) ReleaseCOM(it.second);
	for (pair<const string, IDWriteTextFormat*>& it : fontList) ReleaseCOM(it.second);
	ReleaseCOM(d2Rtg);
	ReleaseCOM(pBackBuffer);
}

void D2Ddevice::CreateD2DDWFactory()
{
	D2D1_FACTORY_OPTIONS options{};
#if defined(DEBUG) || defined(_DEBUG)  
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, d2dFactory.GetAddressOf()));

	HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory), (IUnknown**)dwFactory.GetAddressOf()));
}

void D2Ddevice::InitFonts()
{
	IDWriteTextFormat* format;
	HR(dwFactory->CreateTextFormat(
		L"",
		0,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		DefaultFontSize,
		L"ko",
		&format));
	fontList.insert(make_pair(FontName::DefaultFont, format));
}

//get backbuffer from swapchain and set 2d Rendertarget
void D2Ddevice::ResetBackBuffer(IDXGISwapChain* swapChain)
{
	swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

	FLOAT dpi = (FLOAT)GetDpiForWindow(App->MainWnd());
	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpi,
			dpi
		);

	d2dFactory->CreateDxgiSurfaceRenderTarget(pBackBuffer, &props, &d2Rtg);
}

//idk why but you should release backbuffer and rendertarget before resize swapchain buffer
void D2Ddevice::ResetBackBuffer_Release()
{
	ReleaseCOM(pBackBuffer);
	ReleaseCOM(d2Rtg);
}


ID2D1SolidColorBrush*& D2Ddevice::GetSolidBrush(const D2D1::ColorF color)
{
	//in case not found, create a solid brush
	if (brushList.find(color) == brushList.end())
	{
		ID2D1SolidColorBrush* brush;
		d2Rtg->CreateSolidColorBrush(color, &brush);
		brushList.insert(make_pair(color, move(brush)));
	}

	return brushList[color];
}

IDWriteTextFormat*& D2Ddevice::GetFont(const string name)
{
	//in case not found, returns default font
	if (fontList.find(name) == fontList.end()) return fontList[FontName::DefaultFont];
	else return fontList[name];
}

