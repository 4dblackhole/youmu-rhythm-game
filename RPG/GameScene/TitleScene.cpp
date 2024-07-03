#include "framework.h"
#include "TitleScene.h"

TitleScene::TitleScene() : 
	titleLogoImg(0.0f, 135.0f, 360.0f, 360.0f), 
	background(0, 0, (float)StandardWidth, (float)StandardHeight, { 0,0,0,1 }, true)
{
}

TitleScene::~TitleScene()
{
	
}

void TitleScene::BeginScene(float newW, float newH)
{
	HR(D3DX11CreateShaderResourceViewFromFile(App->GetDevice(),
		L"Textures/Title/TitleLogo_ghost.png", 0, 0, &titleSrv, 0));
	titleLogoImg.SetTexture(titleSrv);

	OnResize(newW, newH);
}

void TitleScene::OnResize(float newW, float newH)
{
	background.Resize(newW, newH);
}

void TitleScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) SCENEMANAGER.ChangeScene(SceneManager::Name::Intro);

}

void TitleScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	background.Render(deviceContext, cam);
	titleLogoImg.Render(deviceContext, cam);
}

void TitleScene::EndScene()
{
	ReleaseCOM(titleSrv);
}
