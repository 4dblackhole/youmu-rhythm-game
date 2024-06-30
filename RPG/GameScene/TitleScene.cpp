#include "framework.h"
#include "TitleScene.h"

TitleScene::TitleScene() : titleImg(0.0f, 0.0f, 557.0f, 174.0f), background()
{
	background.Diffuse = { 0.5f,0.5f,0.5f,1.0f };
}

TitleScene::~TitleScene()
{
	ReleaseCOM(titleSrv);
}

void TitleScene::BeginScene()
{
	HR(D3DX11CreateShaderResourceViewFromFile(App->GetDevice(),
		L"Textures/Title/title.png", 0, 0, &titleSrv, 0));
	titleImg.SetTexture(titleSrv);
}

void TitleScene::OnResize(float newW, float newH)
{
	background.Resize(newW, newH);
}

void TitleScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE))SCENEMANAGER.ChangeScene(SceneManager::Name::Intro);
}

void TitleScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	background.Render(deviceContext, cam);
	titleImg.Render(deviceContext, cam);
}

void TitleScene::EndScene()
{
	ReleaseCOM(titleSrv);
}
