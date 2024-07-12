#include "framework.h"
#include "MainApp.h"

#include "Singleton/TextureManager.h"
#include "Singleton/SceneManager.h"

#include "GameScene/TitleScene.h"
#include "GameScene/SelectMusicScene.h"

MainApp::MainApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mCamera(mClientWidth, mClientHeight),
	drawFPSCounterFlag(true)
{
	mMainWndCaption = _T("2D Test");

	LayoutDesc tempDesc(20.0f, D2D1::ColorF(1, 1, 1, 1), { -10, StandardHeight - 30 });
	tempDesc.alignX = AlignModeX::Right;
	fpsLayout = new DwLayout(tempDesc);
	fpsLayout->SetLayoutRightAlign(L"FPS: 000", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	fpsTimer.Reset();
}

MainApp::~MainApp()
{
	Effects::Release();
	delete fpsLayout;
}

void MainApp::InitGameScenes()
{
	GameScene* scene = new TitleScene;
	SCENEMANAGER.AddScene(SceneManager::Name::Title, scene);
	scene = new SelectMusicScene;
	SCENEMANAGER.AddScene(SceneManager::Name::SelectMusic, scene);
}

bool MainApp::Init()
{
	if (!__super::Init())return false;

	RenderStates::Init(md3dDevice);
	Effects::Init(md3dDevice);

	InitGameScenes();
	BuildBuffer();
	BuildLayout();

	//2D Graphic Setting (no depth check, alpha blend)
	md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS.Get(), {}, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDepthDSS.Get(), 0);

	return true;
}

void MainApp::OnResize()
{
	__super::OnResize();
	mCamera.UpdateProj((float)mClientWidth * ((float)StandardHeight / (float)mClientHeight), (float)StandardHeight);
	SCENEMANAGER.GetCurrentScene()->OnResize((float)mClientWidth, (float)mClientHeight);
	
	fpsLayout->Resize((float)mClientWidth, (float)mClientHeight);
}

void MainApp::UpdateScene(float dt)
{
	SCENEMANAGER.GetCurrentScene()->Update(dt);
	if (KEYBOARD.Press(VK_CONTROL) && KEYBOARD.Down(VK_F7)) drawFPSCounterFlag = !drawFPSCounterFlag;
}

void MainApp::UpdateFPS()
{
	static int frameCnt;
	static float timeElapsed;
	constexpr float calculateInterval = 0.5f;
	constexpr float intervalInverse = 1.0f / calculateInterval;
	
	fpsTimer.Tick();
	++frameCnt;

	if ((fpsTimer.TotalTime() - timeElapsed) >= calculateInterval)
	{
		const float fps = (float)frameCnt * intervalInverse;

		WCHAR fpsStr[20];
		swprintf_s(fpsStr, L"FPS: %.f", fps);
		fpsLayout->SetLayout(fpsStr, D2D.GetFont(D2Ddevice::FontName::DefaultFont));

		//frame calculation end
		frameCnt = 0;
		timeElapsed += calculateInterval;
	}

}

void MainApp::DrawScene()
{
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, (const float*)Colors::LightSteelBlue);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	D2D.BeginDraw();

	SCENEMANAGER.GetCurrentScene()->Render(md3dImmediateContext, mCamera);

	if (drawFPSCounterFlag)
	{
		UpdateFPS();
		fpsLayout->Draw();
	}
	D2D.EndDraw();
	HR(mSwapChain->Present(0, 0));
}

void MainApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	SetCapture(mhMainWnd);
	SCENEMANAGER.GetCurrentScene()->OnMouseDown(btnState, x, y);
}

void MainApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
	SCENEMANAGER.GetCurrentScene()->OnMouseUp(btnState, x, y);
}

void MainApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	SCENEMANAGER.GetCurrentScene()->OnMouseMove(btnState, x, y);
}

void MainApp::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	SCENEMANAGER.GetCurrentScene()->OnKeyDown(wParam, lParam);
}

void MainApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	SCENEMANAGER.GetCurrentScene()->OnKeyUp(wParam, lParam);
}

void MainApp::BuildBuffer()
{
	Sprite::BulidBuffer(md3dDevice);
}

void MainApp::BuildLayout()
{
	Sprite::BuildLayout(md3dDevice);
}
