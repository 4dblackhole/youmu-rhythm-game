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
	vector<thread>initThreads;
	initThreads.emplace_back(thread(&TitleScene::InitLayout, this));
	initThreads.emplace_back(thread(&TitleScene::InitFmod, this));
	for (thread& it : initThreads) it.join();
}

TitleScene::~TitleScene()
{
}

void TitleScene::BeginScene()
{
	HR(D3DX11CreateShaderResourceViewFromFile(App->GetDevice(),
		(TextureDir+L"/Title/TitleLogo_ghost.png").c_str(), 0, 0, &titleSrv, 0));
	titleLogoImg.SetTexture(titleSrv);
	keySelectTriangle = new Triangle2D;
	keySelectTriangle->GetWorld2d().SetScale({12.0f,12.0f});
	keySelectTriangle->FillColor = MyColorF::GhostGreen;
	keySelectTriangle->BorderSize = 0.5f;
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
			SendMessage(App->MainWnd(), WM_DESTROY, 0, 0);
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
		keySelectTriangle->GetWorld2d().SetPosition({ selectKeyPosX , gamestartPosY + selectKeyOffsetY });
		break;
	case TitleScene::SelectKey::Options:
		keySelectTriangle->GetWorld2d().SetPosition({ selectKeyPosX , optionPosY + selectKeyOffsetY });
		break;
	case TitleScene::SelectKey::Exit:
		keySelectTriangle->GetWorld2d().SetPosition({ selectKeyPosX , exitPosY + selectKeyOffsetY });
		break;
	}
}

void TitleScene::InitLayout()
{
	layoutList.reserve((size_t)LayoutKey::MAX);

	LayoutDesc tempDesc(40.0f, MyColorF::GhostGreen, { layoutStartPosX, layoutStartPosY });

	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::GameStart].SetLayout(L"GameStart", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	tempDesc.world2d.SetPosition({ tempDesc.world2d.GetPosition().x, optionPosY });
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Options].SetLayout(L"Options", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	tempDesc.world2d.SetPosition({ tempDesc.world2d.GetPosition().x, exitPosY });
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Exit].SetLayout(L"Exit", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

	tempDesc.Color = D2D1::ColorF({ 1, 1, 1, 1 });
	tempDesc.world2d.SetScale(20.0f);
	tempDesc.world2d.SetPosition({ (float)ToolTipOffsetX,(float)StandardHeight - 30 });
	tempDesc.world2d.alignX = AlignModeX::Left;
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Tooltip].SetLayout(L"Press Enter to Select", D2D.GetFont(D2Ddevice::FontName::DefaultFont));

}

void TitleScene::InitFmod()
{
	fmodSystem.Init();
	fmodSystem.Release();
	fmodSystem.Init();
}
