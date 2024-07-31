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
	for (thread& it : initThreads) it.join();

	FMODSYSTEM.System()->createStream("titlebgm.mp3", FMOD_LOOP_NORMAL, nullptr, &bgm);
}

TitleScene::~TitleScene()
{
	bgmChannel->stop();
	bgm->release();
}

void TitleScene::BeginScene()
{
	HR(D3DX11CreateShaderResourceViewFromFile(App->GetDevice(),
		(TextureDir+L"/Title/TitleLogo_ghost.png").c_str(), 0, 0, &titleSrv, 0));
	titleLogoImg.SetTexture(titleSrv);
	keySelectTriangle = new Triangle2D;
	keySelectTriangle->GetWorld2d().SetScale(12.0f);
	keySelectTriangle->FillColor = MyColorF::GhostGreen;
	keySelectTriangle->BorderSize = 0.5f;
	ChangeTrianglePos();
	keySelectTriangle->GetWorld2d().UpdateWorld();
	keySelectTriangle->GetWorld2d().UpdateGlobalWorld();
	FMODSYSTEM.System()->playSound(bgm, nullptr, false, &bgmChannel);
}

void TitleScene::OnResize(float newW, float newH)
{
	background.ChangeWidthToCurrentWidth(newW, newH);
	tooltipBG.ChangeWidthToCurrentWidth(newW, newH);
	for (DwLayout& it : layoutList) it.Resize(newW, newH);
	keySelectTriangle->Resize(newW, newH);

	int asdf = 3;
}

void TitleScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) SCENEMANAGER.ChangeScene(SceneManager::Name::Intro);
	if (KEYBOARD.Down(VK_UP))
	{
		if (selectKey != SelectKey::GameStart)
		{
			FMODSYSTEM.Play(FmodSystem::Name::button01a);
			int newKey = (int)selectKey - 1;
			selectKey = (SelectKey)newKey;
			ChangeTrianglePos();
			keySelectTriangle->GetWorld2d().UpdateWorld();
			keySelectTriangle->GetWorld2d().UpdateGlobalWorld();
		}
	}
	if (KEYBOARD.Down(VK_DOWN))
	{
		if (selectKey != (SelectKey)((int)SelectKey::MAX - 1))
		{
			FMODSYSTEM.Play(FmodSystem::Name::button01a);
			int newKey = (int)selectKey + 1;
			selectKey = (SelectKey)newKey;
			ChangeTrianglePos();
			keySelectTriangle->GetWorld2d().UpdateWorld();
			keySelectTriangle->GetWorld2d().UpdateGlobalWorld();
		}
	}
	if (KEYBOARD.Down(VK_RETURN))
	{
		FMODSYSTEM.Play(FmodSystem::Name::select05);
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
}

void TitleScene::EndScene()
{
	ReleaseCOM(titleSrv);
	delete keySelectTriangle;
	bgmChannel->stop();
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

	constexpr float selectTextSize = 40.0f / D2Ddevice::DefaultFontSize;
	IDWriteTextFormat*& currentFormat = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	LayoutDesc tempDesc(selectTextSize, MyColorF::GhostGreen, { 0,0 });
	tempDesc.world2d.SetPosition({ layoutStartPosX, layoutStartPosY });
	tempDesc.world2d.UpdateWorld();

	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::GameStart].SetLayout(L"GameStart", currentFormat);

	tempDesc.world2d.SetPosition({ tempDesc.world2d.GetPosition().x, optionPosY });
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Options].SetLayout(L"Options", currentFormat);

	tempDesc.world2d.SetPosition({ tempDesc.world2d.GetPosition().x, exitPosY });
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Exit].SetLayout(L"Exit", currentFormat);

	constexpr float tooltipTextSize = 20.0f / D2Ddevice::DefaultFontSize;
	tempDesc.Color = D2D1::ColorF({ 1, 1, 1, 1 });
	tempDesc.world2d.SetScale(tooltipTextSize);
	tempDesc.world2d.SetPosition({ (float)ToolTipOffsetX,(float)StandardHeight - 30 });
	tempDesc.world2d.alignX = AlignModeX::Left;
	tempDesc.world2d.UpdateWorld();
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Tooltip].SetLayout(L"Press Enter to Select", currentFormat);

}
