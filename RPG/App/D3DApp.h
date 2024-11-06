#pragma once
#include "System/GameTimer.h"

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE AppInst() const;
	HWND MainWnd() const;
	float AspectRatio() const;

	auto GetWidth() const { return mClientWidth; }
	auto GetHeight() const { return mClientHeight; }
	auto GetDevice() const { return md3dDevice; }
	auto GetDeviceContext() const { return  md3dImmediateContext; }
	auto GetSwapChain() const { return  mSwapChain; }

	int Run(); //main thread

	//Framework methods
	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//Mouse input methods
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}
	virtual void OnMouseWheel(WPARAM btnState, int x, int y) {}

	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) {}
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam) {}

	virtual void OnWindowActive();
	virtual void OnWindowInactive();

protected:
	bool InitMainWindow();
	bool InitDirect3D();

	void CalculateFrameStats();

	void GetMonitorFrequency();

protected:

	//window status
	HINSTANCE mhAppInst;
	HWND mhMainWnd;
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;
	BOOL isFullScreen;

	//window size
	int mClientWidth;
	int mClientHeight;

	//4X MSAA config
	UINT m4xMsaaQuality;
	bool mEnable4xMsaa;

	GameTimer mTimer;
	LPCTSTR mMainWndCaption;
	LPCTSTR szWindowClass;

	//devices
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;

	//Driver Type
	D3D_DRIVER_TYPE md3dDriverType;

	//monitor frequency
	DWORD dwMonitorFrequency;
};