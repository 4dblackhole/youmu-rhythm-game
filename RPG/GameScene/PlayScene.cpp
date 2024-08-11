#include "framework.h"
#include "PlayScene.h"

constexpr float LayoutSize = 4.0f;
constexpr int LayoutBaseX = -60;
constexpr int LayoutBaseY = 200;
constexpr int LayoutDistanceY = 100;

constexpr int TriangleOffsetX = -8;
constexpr int TriangleOffsetY = 11;

PlayScene::PlayScene(const Music* m, const Pattern* p) : 
	music(m), pattern(p), 
	transparentBlackBG(0, 0, (float)StandardWidth, (float)StandardHeight, { 0,0,0,0.5f }, true)
{
	InitPauseOptionLayoutList();
}

void PlayScene::InitPauseOptionLayoutList()
{
	IDWriteTextFormat*& defaultFont = D2D.GetFont(D2Ddevice::FontName::DefaultFont);
	for (int i = 0; i < (int)PauseOption::MAX; ++i)
	{
		PauseOptLayoutList[i].GetWorld2d().SetAlignX(AlignModeX::Mid);
		PauseOptLayoutList[i].GetWorld2d().SetScale(LayoutSize);
		PauseOptLayoutList[i].GetWorld2d().SetPosition({ (float)LayoutBaseX,(float)(LayoutBaseY + (i * LayoutDistanceY)) });
		PauseOptLayoutList[i].SetLayout(pauseOptTxtMap[i], defaultFont);
		PauseOptLayoutList[i].GetWorld2d().SetParentDrawWorld();
	}

	pauseOptSelectTriangle.GetWorld2d().SetScale(3.0f);
	pauseOptSelectTriangle.GetWorld2d().SetParentWorld(&PauseOptLayoutList[pauseOptionKey].GetWorld2d());
	pauseOptSelectTriangle.GetWorld2d().SetAlignX(PauseOptLayoutList[pauseOptionKey].GetWorld2d().GetAlignX());
	pauseOptSelectTriangle.FillColor = MyColorF::GhostGreen;
	pauseOptSelectTriangle.BorderSize = 0.5f;
	pauseOptSelectTriangle.GetWorld2d().SetPosition({ (float)TriangleOffsetX, (float)TriangleOffsetY });
}

PlayScene::~PlayScene()
{
}

void PlayScene::BeginScene()
{
}

void PlayScene::OnResize(float newW, float newH)
{
	transparentBlackBG.ChangeWidthToCurrentWidth(newW, newH);
}

void PlayScene::ChangeStatusReady()
{
	sceneStatus = Status::Ready;
}

void PlayScene::ChangeStatusStart()
{
	sceneStatus = Status::Start;
}

void PlayScene::ChangeStatusResume()
{
	sceneStatus = Status::Resume;
	resumeTimer.Reset();
}

void PlayScene::ChangeStatusPause()
{
	sceneStatus = Status::Pause;
}

void PlayScene::ChangeStatusEnd()
{
	sceneStatus = Status::End;
}

void PlayScene::UpdateOnReady(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) ChangeStatusPause();
}

void PlayScene::UpdateOnStart(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) ChangeStatusPause();
}

void PlayScene::UpdateOnResume(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) ChangeStatusPause();

	resumeTimer.Tick();
	if (resumeTimer.TotalTime() >= 1.0f) ChangeStatusReady();
}

void PlayScene::ChangePauseOptionKey(int val)
{
	FMODSYSTEM.Play(FmodSystem::Name::button01a);
	pauseOptionKey = val;
	pauseOptSelectTriangle.GetWorld2d().SetParentWorld(&PauseOptLayoutList[pauseOptionKey].GetWorld2d());
}

void PlayScene::UpdateOnPause(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE))sceneStatus = Status::Resume;

	if (KEYBOARD.Down(VK_UP))
	{
		if (pauseOptionKey > (int)PauseOption::Resume)
		{
			ChangePauseOptionKey(pauseOptionKey - 1);
		}
		else if (pauseOptionKey == (int)PauseOption::Resume)
		{
			ChangePauseOptionKey((int)PauseOption::Quit);
		}
	}
	if (KEYBOARD.Down(VK_DOWN))
	{
		if (pauseOptionKey < (int)PauseOption::Quit)
		{
			ChangePauseOptionKey(pauseOptionKey + 1);
		}
		else if (pauseOptionKey == (int)PauseOption::Quit)
		{
			ChangePauseOptionKey((int)PauseOption::Resume);
		}
	}

	if (KEYBOARD.Down(VK_RETURN))
	{
		switch (pauseOptionKey)
		{
		case (int)PauseOption::Resume:
			FMODSYSTEM.Play(FmodSystem::Name::select05);
			ChangeStatusResume();
			break;

		case (int)PauseOption::Restart:
			//TODO: Reload Pattern
			FMODSYSTEM.Play(FmodSystem::Name::select05);
			ChangeStatusResume();
			break;

		case (int)PauseOption::Quit:
			FMODSYSTEM.Play(FmodSystem::Name::select05);
			ChangeStatusEnd();
			break;
		}
	}
}

void PlayScene::UpdateOnEnd(float dt)
{
	SCENEMANAGER.ChangeScene(SceneManager::Name::SelectMusic);
}

void PlayScene::RenderOnPause(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	transparentBlackBG.Render(deviceContext, cam);
	for (auto& it : PauseOptLayoutList) it.Draw();
	pauseOptSelectTriangle.Draw();

	int asdf = 3;
}


void PlayScene::Update(float dt)
{
	switch (sceneStatus)
	{
	case Status::Ready:
		UpdateOnReady(dt);
		break;
	case Status::Start:
		UpdateOnStart(dt);
		break;
	case Status::Resume:
		UpdateOnResume(dt);
		break;
	case Status::Pause:
		UpdateOnPause(dt);
		break;
	case Status::End:
		UpdateOnEnd(dt);
		break;
	}
}

void PlayScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	switch (sceneStatus)
	{
	case PlayScene::Status::Ready:
		break;
	case PlayScene::Status::Start:
		break;
	case PlayScene::Status::Resume:
		break;
	case PlayScene::Status::Pause:
		RenderOnPause(deviceContext, cam);
		break;
	case PlayScene::Status::End:
		break;
	default:
		break;
	}
}

void PlayScene::EndScene()
{
	SCENEMANAGER.RemoveScene(SceneManager::Name::PlayScene);
}
