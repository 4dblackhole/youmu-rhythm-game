#include "framework.h"
#include "TitleScene.h"

constexpr int layoutStartPosX = -80;
constexpr int layoutStartPosY = 460;
constexpr int layoutLineDistance = 60;

constexpr int gamestartPosY = layoutStartPosY;
constexpr int optionPosY = layoutStartPosY + layoutLineDistance;
constexpr int exitPosY = layoutStartPosY + layoutLineDistance*2;

constexpr float selectKeyPosX = layoutStartPosX - 30.0f;
constexpr float selectKeyOffsetY = 29.0f;

constexpr int ToolTipOffsetX = 8;

constexpr float ToolTipSize = 30.0f;

constexpr XMFLOAT4 DefaultTooltipBgColor{ 0.04f,0.04f,0.04f,1.0f };

TitleScene::TitleScene() :
	titleLogoImg(0.0f, 135.0f, 360.0f, 360.0f),
	background(0, 0, (float)StandardWidth, (float)StandardHeight, { 0, 0, 0, 1 }, true),
	tooltipBG(0, (ToolTipSize -(float)(StandardHeight))*0.5f, (float)StandardWidth, ToolTipSize, DefaultTooltipBgColor, true)
{
	InitLayout();
}

TitleScene::~TitleScene()
{
}

void TitleScene::BeginScene()
{
	HR(D3DX11CreateShaderResourceViewFromFile(App->GetDevice(),
		L"Textures/Title/TitleLogo_ghost.png", 0, 0, &titleSrv, 0));
	titleLogoImg.SetTexture(titleSrv);
	keySelectTriangle = new Triangle2D;
	keySelectTriangle->SetScale({ 12.0f,12.0f });
	keySelectTriangle->FillColor = MyColorF::GhostGreen;
	ChangeTrianglePos();

}

void TitleScene::OnResize(float newW, float newH)
{
	background.ChangeWidthToCurrentWidth(newW, newH);
	tooltipBG.ChangeWidthToCurrentWidth(newW, newH);
	for (DwLayout& it : layoutList) it.Resize(newW, newH);
	keySelectTriangle->Resize(newW, newH);
}

void TitleScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) SCENEMANAGER.ChangeScene(SceneManager::Name::Intro);
	if (KEYBOARD.Down(VK_UP))
	{
		if (selectKey != SelectKey::GameStart)
		{
			int newKey = (int)selectKey - 1;
			selectKey = (SelectKey)newKey;
			ChangeTrianglePos();
		}
	}
	if (KEYBOARD.Down(VK_DOWN))
	{
		if (selectKey != (SelectKey)((int)SelectKey::MAX - 1))
		{
			int newKey = (int)selectKey + 1;
			selectKey = (SelectKey)newKey;
			ChangeTrianglePos();
		}
	}
	if (KEYBOARD.Down(VK_RETURN))
	{
		switch (selectKey)
		{
		case TitleScene::SelectKey::GameStart:
			SCENEMANAGER.ChangeScene(SceneManager::Name::SelectMusic);
			break;
		case TitleScene::SelectKey::Options:
			SCENEMANAGER.ChangeScene(SceneManager::Name::test);
			break;
		case TitleScene::SelectKey::Exit:
			PostQuitMessage(0);
			break;
		}
	}
}

void TitleScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	background.Render(deviceContext, cam);
	tooltipBG.Render(deviceContext, cam);
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

void TitleScene::ChangeTrianglePos()
{
	switch (selectKey)
	{
	case TitleScene::SelectKey::GameStart:
		keySelectTriangle->SetPosition({ selectKeyPosX , gamestartPosY + selectKeyOffsetY });
		break;
	case TitleScene::SelectKey::Options:
		keySelectTriangle->SetPosition({ selectKeyPosX , optionPosY + selectKeyOffsetY });
		break;
	case TitleScene::SelectKey::Exit:
		keySelectTriangle->SetPosition({ selectKeyPosX , exitPosY + selectKeyOffsetY });
		break;
	}
}

void TitleScene::InitLayout()
{
	layoutList.reserve((size_t)LayoutKey::MAX);

	LayoutDesc tempDesc(40.0f, MyColorF::GhostGreen, { layoutStartPosX, layoutStartPosY });

	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::GameStart].SetLayout(L"GameStart", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	tempDesc.Pos.y = optionPosY;
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Options].SetLayout(L"Options", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	tempDesc.Pos.y = exitPosY;
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Exit].SetLayout(L"Exit", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	tempDesc.Color = D2D1::ColorF({ 1, 1, 1, 1 });
	tempDesc.FontSize = 20.0f;
	tempDesc.Pos.x = ToolTipOffsetX;
	tempDesc.Pos.y = StandardHeight - 30;
	tempDesc.DrawPos = { (float)tempDesc.Pos.x, (float)tempDesc.Pos.y };
	tempDesc.alignX = AlignModeX::Left;
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Tooltip].SetLayout(L"Press Enter to Select", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

}
