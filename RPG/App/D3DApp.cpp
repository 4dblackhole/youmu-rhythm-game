#include "framework.h"
#include "d3dApp.h"

#define AlertBox(x) (MessageBox(NULL,(x),NULL,NULL))
static constexpr UINT SWAPCHAIN_BUFFERCOUNT = 2;

namespace
{
	D3DApp* gd3dApp = nullptr;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gd3dApp->MsgProc(hWnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance)
	: mhAppInst(hInstance),
	mMainWndCaption(_T("D3D11 Application")),
	szWindowClass(_T("D3DWndClassName")),
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mClientWidth(StandardWidth),
	mClientHeight(StandardHeight),
	mEnable4xMsaa(false),
	mhMainWnd(0),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false),
	m4xMsaaQuality(0),

	md3dDevice(nullptr),
	md3dImmediateContext(nullptr),
	mSwapChain(nullptr),
	mDepthStencilBuffer(nullptr),
	mRenderTargetView(nullptr),
	mDepthStencilView(nullptr),
	mScreenViewport({}),
	dwMonitorFrequency(60u)
{
	App = this;
	gd3dApp = this;
}

D3DApp::~D3DApp()
{
	mRenderTargetView->Release();
	mDepthStencilView->Release();
	mDepthStencilBuffer->Release();
	mSwapChain->Release();

	if (md3dImmediateContext)
	{
		md3dImmediateContext->ClearState();
	}
	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);
}

HINSTANCE D3DApp::AppInst() const
{
	return mhAppInst;
}

HWND D3DApp::MainWnd() const
{
	return mhMainWnd;
}

float D3DApp::AspectRatio() const
{
	return (float)((double)mClientWidth / (double)mClientHeight);
}

//main thread
int D3DApp::Run()
{
	MSG msg{};
	mTimer.Reset();
	
	while (msg.message != WM_QUIT)
	{
		using namespace chrono;
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			mTimer.Tick();

			if (!mAppPaused)
			{
				KEYBOARD.Update();
				CalculateFrameStats();
				UpdateScene(mTimer.DeltaTime());
				
				//render limit according to monitor hz
				static float renderTime = 0.0f;
				renderTime += mTimer.DeltaTime();
				const float fpsLimit = 1.0f / (float)dwMonitorFrequency;
				if (renderTime >= fpsLimit)
				{
					DrawScene();
					if (renderTime < 1.0f)renderTime -= fpsLimit; 
					else renderTime = 0.0f;
				}
			}
			//no msg and app paused
			else std::this_thread::sleep_for(milliseconds(100));
		}
	}

	return (int)msg.wParam;
}

bool D3DApp::Init()
{
	if (!InitMainWindow()) return false;
	if (!InitDirect3D()) return false;

	return true;
}

void D3DApp::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	//Release the old views
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);

	//Release D2 BackBuffer and RenderTarget
	D2D.ResetBackBuffer_Release();

	//Resize the swap chain and recreate the RTV
	HR(mSwapChain->ResizeBuffers(SWAPCHAIN_BUFFERCOUNT, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	ComPtr<ID3D11Texture2D> backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	HR(md3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &mRenderTargetView));

	//Direct Write
	D2D.ResetBackBuffer(mSwapChain);

	//Create the depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Use 4x MSAA
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	//No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));

	//Bind the Render Target View and Depth/Stencil View to the pipeline
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	//Set the Viewport transform
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = (float)mClientWidth;
	mScreenViewport.Height = (float)mClientHeight;
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	//Window Activate/Inactivate
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

	case WM_SIZE:
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (md3dDevice)
		{
			switch (wParam)
			{
			case SIZE_MINIMIZED:
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
				break;

			case SIZE_MAXIMIZED:
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
				break;

			case SIZE_RESTORED:
				//restoring from minimized state
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else OnResize();

				break;
			default:
				break;
			}
		}
		return 0;

	// when grabs the resize bars
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		return 0;

	case WM_KEYUP:
		OnKeyUp(wParam, lParam);
		return 0;

	case WM_WINDOWPOSCHANGING:
		GetMonitorFrequency();
		return 0;

	default: break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = szWindowClass;

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, _T("RegisterClass Failed."), nullptr, NULL);
		return false;
	}

	//compute window rectangle dimensions based on requested cliend area dimensions
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	constexpr DWORD dwStyle = (WS_OVERLAPPEDWINDOW /* & (~WS_THICKFRAME) */ );
	AdjustWindowRect(&R, dwStyle, false);
	int w = R.right - R.left;
	int h = R.bottom - R.top;

	mhMainWnd = CreateWindow(szWindowClass, mMainWndCaption,
		dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, mhAppInst, NULL);

	if (!mhMainWnd)
	{
		MessageBox(NULL, _T("CreateWindow Failed."), NULL, NULL);
		return FALSE;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

bool D3DApp::InitDirect3D()
{
	//Create the device and device context

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		nullptr,
		md3dDriverType,
		NULL,
		createDeviceFlags,
		nullptr, NULL,
		D3D11_SDK_VERSION,
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);

	if (FAILED(hr))
	{
		AlertBox(_T("D3D11CreateDevice Failed"));
		return false;
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		AlertBox(_T("Direct3D Feature Level 11 unsupported"));
		return false;
	}

	//check 4X MSAA quality support for our back buffer format
	HR(md3dDevice->CheckMultisampleQualityLevels
	(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	//Fill out a DXGI_SWAPCHAIN_DESC to describe our swap chain

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//usa 4xmsaa?
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SWAPCHAIN_BUFFERCOUNT;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = 0;

	//To correctly create the swap chain, we must use the IDXGIFactory
	IDXGIDevice* dxgiDevice{};
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter{};
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory{};
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));
	//Alt-Enter Disable
	//HR(dxgiFactory->MakeWindowAssociation(mhMainWnd, DXGI_MWA_NO_ALT_ENTER));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	//Init singletons
	TextureManager::GetInstance().InitTextures(md3dDevice);
	SceneManager::GetInstance();
	D2Ddevice::GetInstance();

	OnResize();
	return true;
}

void D3DApp::CalculateFrameStats()
{
	static int frameCnt;
	static float timeElapsed;
	constexpr float calculateInterval = 1.0f;

	++frameCnt;

	if ((mTimer.TotalTime() - timeElapsed) >= calculateInterval)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;

		tostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << _T("   UPS: ") << fps << L"    "
			<< _T("Update Time: ") << mspf << _T(" (ms)");
		SetWindowText(mhMainWnd, outs.str().c_str());

		//frame calculation end
		frameCnt = 0;
		timeElapsed += calculateInterval;
	}

}

void D3DApp::GetMonitorFrequency()
{
	DEVMODE devMode{};
	devMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);

	dwMonitorFrequency = devMode.dmDisplayFrequency;
}
