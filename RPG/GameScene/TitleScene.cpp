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
	titleSRV.CreateTexture(App->GetDevice(), TextureDir + L"/Title/TitleLogo_ghost.png");
	titleLogoImg.SetTexture(&titleSRV);
	keySelectTriangle = new Triangle2D;
	keySelectTriangle->GetWorld2d().SetScale(12.0f);
	keySelectTriangle->GetWorld2d().SetAlignX(AlignModeX::Mid);
	keySelectTriangle->isHollow = false;
	keySelectTriangle->FillColor = MyColorF::GhostGreen;
	keySelectTriangle->BorderSize = 0.5f;
	keySelectTriangle->GetWorld2d().SetParentDrawWorld();
	ChangeTrianglePos(selectKey);
	FMODSYSTEM.System()->playSound(bgm, nullptr, false, &bgmChannel);
}

void TitleScene::OnResize(float newW, float newH)
{
	tooltipBG.ChangeWidthToCurrentWidth(newW, newH);

	keySelectTriangle->GetWorld2d().OnParentWorldUpdate();
	for (auto& it : layoutList) it.GetWorld2d().OnParentWorldUpdate();
}

void TitleScene::Update(float dt)
{
	if(KEYBOARD.Hold(VK_F1)) SCENEMANAGER.ChangeScene(SceneManager::Name::test);

	if (KEYBOARD.Down(VK_ESCAPE)) SCENEMANAGER.ChangeScene(SceneManager::Name::Logo);
	if (KEYBOARD.Down(VK_UP))
	{
		FMODSYSTEM.Play(FmodSystem::Name::button01a);
		if (selectKey != SelectKey::GameStart)
		{
			ChangeTrianglePos((SelectKey)((int)selectKey - 1));
		}
		else
		{
			ChangeTrianglePos((SelectKey)((int)SelectKey::MAX - 1));
		}
	}
	if (KEYBOARD.Down(VK_DOWN))
	{
		FMODSYSTEM.Play(FmodSystem::Name::button01a);
		if (selectKey != (SelectKey)((int)SelectKey::MAX - 1))
		{
			ChangeTrianglePos((SelectKey)((int)selectKey + 1));
		}
		else
		{
			ChangeTrianglePos((SelectKey)0);
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
	tooltipBG.Render(deviceContext, cam);
	titleLogoImg.Render(deviceContext, cam);

	for (auto& it : layoutList) it.Draw();
	keySelectTriangle->Draw();
}

void TitleScene::EndScene()
{
	ReleaseCOM(titleSRV.GetRefSRV());
	delete keySelectTriangle;
	bgmChannel->stop();
}

void TitleScene::ChangeTrianglePos(SelectKey key)
{
	selectKey = key;
	switch (key)
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
	DwLayout2D tempDesc(selectTextSize, MyColorF::GhostGreen, { 0,0 });
	tempDesc.GetWorld2d().SetAlignX(AlignModeX::Mid);
	tempDesc.GetWorld2d().SetPosition({ layoutStartPosX, layoutStartPosY });

	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::GameStart].SetLayout(L"GameStart", currentFormat);

	tempDesc.GetWorld2d().SetPosition({ tempDesc.GetWorld2d().GetPosition().x, optionPosY });
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Options].SetLayout(L"Options", currentFormat);

	tempDesc.GetWorld2d().SetPosition({ tempDesc.GetWorld2d().GetPosition().x, exitPosY });
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Exit].SetLayout(L"Exit", currentFormat);

	constexpr float tooltipTextSize = 20.0f / D2Ddevice::DefaultFontSize;
	tempDesc.Color = D2D1::ColorF({ 1, 1, 1, 1 });
	tempDesc.GetWorld2d().SetScale(tooltipTextSize);
	tempDesc.GetWorld2d().SetPosition({ (float)ToolTipOffsetX,(float)StandardHeight - 30 });
	tempDesc.GetWorld2d().SetAlignX(AlignModeX::Left);
	layoutList.emplace_back(tempDesc);
	layoutList[(size_t)LayoutKey::Tooltip].SetLayout(L"Press Enter to Select", currentFormat);

	for (auto& it : layoutList)it.GetWorld2d().SetParentDrawWorld();

}
