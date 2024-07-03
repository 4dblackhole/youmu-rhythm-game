#include "framework.h"
#include "MainApp.h"

#include "Singleton/TextureManager.h"
#include "Singleton/SceneManager.h"

#include "GameScene/TitleScene.h"

MainApp::MainApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mCamera(mClientWidth, mClientHeight)
{
	mMainWndCaption = _T("2D Test");

}

MainApp::~MainApp()
{
	Effects::Release();
	for (auto& it : layoutList)	ReleaseCOM(it.second);
}

void MainApp::InitGameScenes()
{
	GameScene* scene = new TitleScene;
	SCENEMANAGER.AddScene(SceneManager::Name::Title, scene);
}

bool MainApp::Init()
{
	if (!__super::Init())return false;

	RenderStates::Init(md3dDevice);
	Effects::Init(md3dDevice);

	InitGameScenes();

	IDWriteTextLayout* tempLayout;
	HR(DIRECTWRITE.GetDwFactory()->CreateTextLayout(L"ASDF", 4, DIRECTWRITE.GetFont(DirectWrite::FontName::DefaultFont), 100, 20, &tempLayout));
	layoutList.insert(make_pair(0, tempLayout));

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
}

void MainApp::UpdateScene(float dt)
{
	SCENEMANAGER.GetCurrentScene()->Update(dt);
}

void MainApp::DrawScene()
{
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, (const float*)Colors::LightSteelBlue);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DirectWrite::GetInstance().BeginDraw();

	SCENEMANAGER.GetCurrentScene()->Render(md3dImmediateContext, mCamera);
	IDWriteTextLayout* currentLayout = layoutList[0];
	float fontSize = currentLayout->GetFontSize();
	currentLayout->SetFontSize(fontSize* 5.0f, { 0,1 });
	DirectWrite::GetInstance().DrawTextLayout({ 100, 200 }, currentLayout, DIRECTWRITE.GetSolidBrush(D2D1::ColorF(48.0f / 255.0f, 224.0f / 255.0f, 104.0f / 255.0f, 1)));

	DirectWrite::GetInstance().EndDraw();

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
