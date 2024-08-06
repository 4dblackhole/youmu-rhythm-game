#include "framework.h"
#include "MainApp.h"

#include "Singleton/TextureManager.h"
#include "Singleton/SceneManager.h"

#include "GameScene/TitleScene.h"
#include "GameScene/SelectMusicScene.h"

const int FPScounterY = StandardHeight - 30;

MainApp* App;

MainApp::MainApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mCamera(mClientWidth, mClientHeight),
	drawFPSCounterFlag(true)
{
	App = this;
	mMainWndCaption = _T("2D Test");

	constexpr float fpsFontSize = 20.0f / D2Ddevice::DefaultFontSize;
	fpsLayout = new DwLayout2D(fpsFontSize, D2D1::ColorF(1, 1, 1, 1), { 0 ,0 });
	fpsLayout->GetWorld2d().SetAlignX(AlignModeX::Right);
	fpsLayout->GetWorld2d().SetPosition({ -10 ,(float)FPScounterY });
	fpsLayout->SetLayoutRightAlign(L"FPS: 000", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	fpsTimer.Reset();

	InitDrawWorld();
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

void MainApp::NotifyDrawWorldResize()
{
	for (int idx = (int)AlignModeX::Mid; idx < (int)AlignModeX::MAX; ++idx) mDrawWorldArr[idx].second = true;
}

void MainApp::OnResize()
{
	__super::OnResize();
	NotifyDrawWorldResize();
	mCamera.UpdateProj((float)mClientWidth * ((float)StandardHeight / (float)mClientHeight), (float)StandardHeight);
	rateY = (float)GetHeight() / (float)StandardHeight;
	SCENEMANAGER.GetCurrentScene()->OnResize((float)mClientWidth, (float)mClientHeight);
	
	//fpsLayout->Resize((float)mClientWidth, (float)mClientHeight);
	int asdf = 3;
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

void MainApp::InitDrawWorld()
{
	//always I matrix
	mDrawWorldArr[(int)AlignModeX::Left].first = D2D1::Matrix3x2F::Identity();
	mDrawWorldArr[(int)AlignModeX::Left].second = false;

	for (int x = (int)AlignModeX::Mid; x < (int)AlignModeX::MAX; ++x)
	{
		//mDrawWorldArr[x].first = D2D1::Matrix3x2F::Identity();
		mDrawWorldArr[x].second = true;
	}
}

void MainApp::UpdateDrawWorld(AlignModeX x)
{
	const int xIdx = (int)x;
	if (mDrawWorldArr[xIdx].second)
	{
		D2D1_POINT_2F drawPos;
		float w = (float)mClientWidth;
		float h = (float)mClientHeight;
		switch (x)
		{
		case AlignModeX::Left:
			drawPos = { 0, 0 };
			break;
		case AlignModeX::Mid:
			drawPos = ShortCut::Resize2DtoStandardCS(w, h, 0, 0, w * 0.5f);
			break;
		case AlignModeX::Right:
			drawPos = ShortCut::Resize2DtoStandardCS(w, h, 0, 0, w);
			break;
		default:
			drawPos = { 0, 0 };
			break;
		}
		mDrawWorldArr[xIdx].first = D2D1::Matrix3x2F::Scale({ rateY, rateY }) * D2D1::Matrix3x2F::Translation({ drawPos.x, drawPos.y });
		mDrawWorldArr[xIdx].second = false;
		TRACE(_T("DrawWorld Updated\n"));
	}
}

const D2D1::Matrix3x2F& MainApp::GetDrawWorld(const AlignModeX x)
{
	UpdateDrawWorld(x);
	return mDrawWorldArr[(int)x].first;
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

void MainApp::BuildBuffer()
{
	Sprite::BulidBuffer(md3dDevice);
}

void MainApp::BuildLayout()
{
	Sprite::BuildLayout(md3dDevice);
}
