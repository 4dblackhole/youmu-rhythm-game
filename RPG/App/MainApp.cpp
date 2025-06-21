#include "framework.h"
#include "MainApp.h"

#include "Singleton/SceneManager.h"

#include "GameScene/TitleScene.h"
#include "GameScene/SelectMusicScene.h"

const int FPScounterY = StandardHeight - 30;

MainApp* App;

MainApp::MainApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mCamera(mClientWidth, mClientHeight),
	drawFPSCounterFlag(true),
	bgColor(DefaultBgColor)
{
	App = this;
	mMainWndCaption = _T("2D Test");

	constexpr float fpsFontSize = 20.0f / D2Ddevice::DefaultFontSize;
	fpsLayout = new DwLayout2D(fpsFontSize, D2D1::ColorF(1, 1, 1, 1), { 0 ,0 });
	fpsLayout->GetWorld2d().SetAlignX(AlignModeX::Right);
	fpsLayout->GetWorld2d().SetPosition({ -10 ,(float)FPScounterY });
	fpsLayout->GetWorld2d().SetParentDrawWorld();
	fpsLayout->SetLayoutRightAlign(L"FPS: 000", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	fpsTimer.Reset();

}

MainApp::~MainApp()
{
	EffectList::Release();
	Sprite::ReleaseSpriteBuffer();
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

	//Init singletons
	SceneManager::GetInstance();
	StrToVK::GetInstance();

	RenderStates::Init(md3dDevice);
	EffectList::Init(md3dDevice);

	InitGameScenes();
	BuildBuffer();
	BuildLayout();

	//2D Graphic Setting (no depth check, alpha blend)
	md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS.Get(), {}, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDepthDSS.Get(), 0);

	FMODSYSTEM.ChangeDriveOutputType(FMOD_OUTPUTTYPE_ASIO);
	FMODSYSTEM.System()->setDSPBufferSize(64, 4);
	FMODSYSTEM.ChangeDrive(0);

	return true;
}

void MainApp::NotifyDrawWorldResize()
{
	for (int ax = 0; ax < (int)AlignModeX::MAX; ++ax) UpdateDrawWorld2D(AlignModeX(ax));

	for (int ay = 0; ay < (int)AlignModeY::MAX; ++ay)
		for (int ax = 0; ax < (int)AlignModeX::MAX; ++ax)
			UpdateDrawWorld3D((AlignModeX)ax, AlignModeY(ay));

}

void MainApp::OnResize()
{
	__super::OnResize();
	mCamera.UpdateProj((float)mClientWidth * ((float)StandardHeight / (float)mClientHeight), (float)StandardHeight);
	rateY = (float)GetHeight() / (float)StandardHeight;

	NotifyDrawWorldResize();
	SCENEMANAGER.GetCurrentScene()->OnResize((float)mClientWidth, (float)mClientHeight);
	
	fpsLayout->GetWorld2d().OnParentWorldUpdate();
}

void MainApp::UpdateScene(float dt)
{
	SCENEMANAGER.GetCurrentScene()->Update(dt);
	if (KEYBOARD.Hold(VK_CONTROL) && KEYBOARD.Down(VK_F7)) drawFPSCounterFlag = !drawFPSCounterFlag;
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
		fpsLayout->SetText(fpsStr);
		//frame calculation end
		frameCnt = 0;
		timeElapsed += calculateInterval;
	}

}

void MainApp::UpdateDrawWorld2D(AlignModeX x)
{
	D2D1_POINT_2F drawPos{};
	const float w = (float)mClientWidth;
	const float h = (float)mClientHeight;
	switch (x)
	{
	case AlignModeX::Mid:
		drawPos = ShortCut::Resize2DtoStandardCS(w, h, 0, 0, w * 0.5f);
		break;
	case AlignModeX::Right:
		drawPos = ShortCut::Resize2DtoStandardCS(w, h, 0, 0, w);
		break;
	}
	mDrawWorld2DArr[(int)x].SetPosition(drawPos);
	mDrawWorld2DArr[(int)x].SetScale(RateY());
}

void MainApp::UpdateDrawWorld3D(AlignModeX x, AlignModeY y)
{
	const int xIdx = (int)x;
	const int yIdx = (int)y;

	XMFLOAT3 resultPos{};
	const float localWidth = ShortCut::GetOrthoWidth((float)App->GetWidth(), (float)App->GetHeight());
	switch (x)
	{
	case AlignModeX::Left:
		resultPos.x = -localWidth * 0.5f;
		break;
	case AlignModeX::Right:
		resultPos.x = localWidth * 0.5f;
		break;
	}
	switch (y)
	{
	case AlignModeY::Top:
		resultPos.y = (float)StandardHeight * 0.5f;
		break;
	case AlignModeY::Bottom:
		resultPos.y = -(float)StandardHeight * 0.5f;
		break;
	}
	mDrawWorld3DArr[yIdx][xIdx].SetLocalPosition(resultPos);
}

World2D& MainApp::GetDrawWorld2D(const AlignModeX x)
{
	return mDrawWorld2DArr[(int)x];
}

World3D& MainApp::GetDrawWorld3D(const AlignModeX x, const AlignModeY y)
{
	return mDrawWorld3DArr[(int)y][(int)x];
}

void MainApp::DrawScene()
{
	ResetRenderTarget();

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, (const float*)&bgColor);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	D2D.BeginDraw();

	SCENEMANAGER.GetCurrentScene()->Render(md3dImmediateContext, mCamera);

	if (drawFPSCounterFlag)
	{
		UpdateFPS();
		fpsLayout->Draw();
	}
	D2D.EndDraw();

	mSwapChain->GetFullscreenState(&isFullScreen, nullptr);
	if (isFullScreen == FALSE)
	{
		HR(mSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));
	}
	else
	{
		HR(mSwapChain->Present(0, 0));
	}
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

void MainApp::OnMouseWheel(WPARAM btnState, int x, int y)
{
	SCENEMANAGER.GetCurrentScene()->OnMouseWheel(btnState, x, y);
}

void MainApp::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	SCENEMANAGER.GetCurrentScene()->OnKeyDown(wParam, lParam);
}

void MainApp::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	SCENEMANAGER.GetCurrentScene()->OnKeyUp(wParam, lParam);
}
void MainApp::ResetRenderTarget()
{
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
}
/*
void MainApp::OnWindowActive()
{
	__super::OnWindowActive();
	SCENEMANAGER.GetCurrentScene()->OnWindowActive();
}

void MainApp::OnWindowInactive()
{
	__super::OnWindowInactive();
	SCENEMANAGER.GetCurrentScene()->OnWindowInactive();
}
*/
void MainApp::BuildBuffer()
{
	Sprite::BulidBuffer(md3dDevice);
}

void MainApp::BuildLayout()
{
	Sprite::BuildLayout(md3dDevice);
}
