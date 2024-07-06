#include "framework.h"
#include "TitleScene.h"

constexpr int layoutStartPosX = -80;
constexpr int layoutStartPosY = 500;
constexpr int layoutLineDistance = 60;

constexpr float selectKeyPosX = layoutStartPosX - 30.0f;
constexpr float selectKeyOffsetY = 28.0f;

TitleScene::TitleScene() :
	titleLogoImg(0.0f, 135.0f, 360.0f, 360.0f),
	background(0, 0, (float)StandardWidth, (float)StandardHeight, { 0, 0, 0, 1 }, true)
{
	InitLayout();
}

TitleScene::~TitleScene()
{
}

void TitleScene::BeginScene(float newW, float newH)
{

	HR(D3DX11CreateShaderResourceViewFromFile(App->GetDevice(),
		L"Textures/Title/TitleLogo_ghost.png", 0, 0, &titleSrv, 0));
	titleLogoImg.SetTexture(titleSrv);
	keySelectTriangle = new Triangle2D;
	keySelectTriangle->SetScale({ 12.0f,12.0f });
	keySelectTriangle->SetPosition({ selectKeyPosX , (float)layoutStartPosY + selectKeyOffsetY });

	OnResize(newW, newH);
}

void TitleScene::OnResize(float newW, float newH)
{
	background.Resize(newW, newH);
	for (DwLayout& it : layoutList)it.Resize(newW, newH);
	keySelectTriangle->Resize(newW, newH);
}

void TitleScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) SCENEMANAGER.ChangeScene(SceneManager::Name::Intro);
	if (KEYBOARD.Press('Z'))
	{
		auto p = keySelectTriangle->GetPosition();
		p.width += 20.0f*dt;
		keySelectTriangle->SetPosition(p);
	}
	if (KEYBOARD.Press('X'))
	{
		auto p = keySelectTriangle->GetPosition();
		p.width -= 20.0f * dt;
		keySelectTriangle->SetPosition(p);
	}
}

void TitleScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	background.Render(deviceContext, cam);
	titleLogoImg.Render(deviceContext, cam);

	for (DwLayout& it : layoutList) it.Draw();
	keySelectTriangle->Draw();
	D2D.GetRenderTarget()->SetTransform(D2D1::Matrix3x2F::Identity());
}

void TitleScene::EndScene()
{
	ReleaseCOM(titleSrv);
	delete keySelectTriangle;
}

void TitleScene::InitLayout()
{
	layoutList.reserve(LayoutKey::MAX);

	LayoutDesc tempDesc(40.0f, D2D1::ColorF(48.0f / 255.0f, 224.0f / 255.0f, 104.0f / 255.0f, 1), { layoutStartPosX, layoutStartPosY });

	layoutList.push_back(tempDesc);
	layoutList[LayoutKey::GameStart].CreateLayout(L"Game Start", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	tempDesc.Pos.y = layoutStartPosY + layoutLineDistance;
	layoutList.push_back(tempDesc);
	layoutList[LayoutKey::Options].CreateLayout(L"Options", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	tempDesc.Pos.y = layoutStartPosY + layoutLineDistance * 2;
	layoutList.push_back(tempDesc);
	layoutList[LayoutKey::Exit].CreateLayout(L"Exit", D2D.GetFont(D2Ddevice::FontName::DefaultFont));
}
