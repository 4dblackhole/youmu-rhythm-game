#include "framework.h"
#include "MainApp.h"

#include "Singleton/TextureManager.h"
#include "Singleton/SceneManager.h"

#include "System/GameScene/TestScene.h"
#include "System/GameScene/BackGroundScene.h"

MainApp::MainApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mCamera(mClientWidth, mClientHeight)
{
	mMainWndCaption = _T("2D Test");

}

MainApp::~MainApp()
{
	Effects::Release();
}

bool MainApp::Init()
{
	if (!__super::Init())return false;

	RenderStates::Init(md3dDevice);
	Effects::Init(md3dDevice);
	TextureManager::GetInstance().InitTextures(md3dDevice);

	TestScene* scene = new TestScene;
	BackGroundScene* scene2 = new BackGroundScene;
	SCENEMANAGER.InitScene("test", move(scene));
	SCENEMANAGER.AddScene("bg", move(scene2));

	BuildBuffer();
	BuildLayout();

	//2D Graphic Setting (no depth check, alpha blend)
	float factor[4]{};

	md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS.Get(), {}, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDepthDSS.Get(), 0);


	return true;
}

void MainApp::OnResize()
{
	__super::OnResize();
	mCamera.UpdateProj((float)mClientWidth * ((float)StandardHeight/ (float)mClientHeight), (float)StandardHeight);
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

	SCENEMANAGER.GetCurrentScene()->Render(md3dImmediateContext, mCamera);

	//sq.Render(md3dImmediateContext, mCamera);
	//sq2.Render(md3dImmediateContext, mCamera);

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

void MainApp::BuildBuffer()
{
	Square::BulidBuffer(md3dDevice);
}

void MainApp::BuildLayout()
{
	Square::BuildLayout(md3dDevice);
}
