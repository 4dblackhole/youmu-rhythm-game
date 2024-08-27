#include "framework.h"
#include "D2Ddevice.h"

D2Ddevice::D2Ddevice()
{
	CreateD2DDWFactory();
	CreateWicFactory();
	InitFonts();
}

D2Ddevice::~D2Ddevice()
{
	for (pair<const D2D1::ColorF, ID2D1SolidColorBrush*>& it : brushList) ReleaseCOM(it.second);
	for (pair<const string, IDWriteTextFormat*>& it : fontList) ReleaseCOM(it.second);
	ReleaseCOM(d2Rtg);
	ReleaseCOM(pBackBuffer);

	CoUninitialize();
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

HRESULT D2Ddevice::CreateWicFactory()
{
	HRESULT hr = S_OK;
	try
	{
		hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (hr == S_FALSE) throw hr;

		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pWicFactory)
		);
		if (FAILED(S_OK)) throw hr;
	}
	catch (HRESULT val)
	{
		val = val;
		CoUninitialize();
	}

	return hr;
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


void D2Ddevice::GetImageDimensions(wstring_view dir, UINT* w, UINT* h)
{
	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
	hr = pWicFactory->CreateDecoderFromFilename(
		dir.data(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		pDecoder.GetAddressOf()
	);
	if (FAILED(hr)) return;

	// 첫 번째 프레임 가져오기 (애니메이션이 아닌 경우 프레임은 하나만 있음)
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pFrame;
	hr = pDecoder->GetFrame(0, pFrame.GetAddressOf());
	if (FAILED(hr)) return;

	// 이미지 해상도 가져오기
	hr = pFrame->GetSize(w, h);

}

//TODO: make this function thread-safe
ID2D1SolidColorBrush*& D2Ddevice::GetSolidBrush(const D2D1::ColorF color)
{
	//in case not found, create a solid brush
	if (brushList.find(color) == brushList.end())
	{
		bool insertCheck = AddSolidBrush(color);
		assert(insertCheck);
	}

	return brushList[color];
}

bool D2Ddevice::AddSolidBrush(const D2D1::ColorF color)
{
	ID2D1SolidColorBrush* brush;
	d2Rtg->CreateSolidColorBrush(color, &brush);
	const auto& result = brushList.emplace(make_pair(color, brush));
	if (!result.second) ReleaseCOM(brush);
	return result.second;
}

//TODO: make this function thread-safe
IDWriteTextFormat*& D2Ddevice::GetFont(const string name)
{
	//in case not found, returns default font
	if (fontList.find(name) == fontList.end()) return fontList[FontName::DefaultFont];
	else return fontList[name];
}

