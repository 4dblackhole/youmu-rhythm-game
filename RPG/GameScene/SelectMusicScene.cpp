#include "framework.h"
#include "GameScene/PlayScene.h"
#include "SelectMusicScene.h"

constexpr int MusicScrollCenterX = -235;
constexpr int MusicScrollCenterY = -345;
constexpr float MusicScrollWidth = 450.0f;
constexpr float MusicScrollHeight = 670.0f;


SelectMusicScene::SelectMusicScene() :
	background(0, 0, (float)StandardWidth, (float)StandardHeight, MyColor4::Black, true),
	musicScroll(MusicScrollCenterX, MusicScrollCenterY, MusicScrollWidth, MusicScrollHeight)
{
	constexpr float tooltipTextSize = 20.0f / D2Ddevice::DefaultFontSize;
	tooltipLayout.SetLayout(L"Music Select: ��/��, Pattern Select: ��/��");
	tooltipLayout.GetWorld2d().SetPosition({ 5 ,(float)StandardHeight - 30 });
	tooltipLayout.GetWorld2d().SetScale(tooltipTextSize);
	tooltipLayout.GetWorld2d().SetAlignX(AlignModeX::Left);
}

SelectMusicScene::~SelectMusicScene()
{
}

void SelectMusicScene::BeginScene()
{
	musicScroll.OnBeginScene();
}

void SelectMusicScene::OnResize(float newW, float newH)
{
	background.ChangeWidthToCurrentWidth(newW, newH);
	musicScroll.OnResize(newW, newH);
	tooltipLayout.OnResize();
}

void SelectMusicScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE))
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::Name::Title);
	}
	musicScroll.Update(dt);

	if (KEYBOARD.Down(VK_RETURN))
	{
		Pattern* currentPattern = musicScroll.GetCurrentPattern();
		Music* currentMusic = musicScroll.GetCurrentMusic();
		if (currentPattern != nullptr)
		{
			FMODSYSTEM.Play(FmodSystem::Name::select05);
			GameScene* playScene = new PlayScene(currentMusic, currentPattern);
			SCENEMANAGER.AddScene(SceneManager::Name::PlayScene, playScene);
			SCENEMANAGER.ChangeScene(SceneManager::Name::PlayScene);
			return;
		}
	}
}

void SelectMusicScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	background.Render(deviceContext, cam);
	musicScroll.Render(deviceContext, cam);
	tooltipLayout.Draw();
}

void SelectMusicScene::EndScene()
{
	musicScroll.OnEndScene();
}

void SelectMusicScene::OnMouseDown(WPARAM btnState, int x, int y)
{
	musicScroll.OnMouseDown(btnState, x, y);
}

void SelectMusicScene::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void SelectMusicScene::OnMouseMove(WPARAM btnState, int x, int y)
{
}

void SelectMusicScene::OnMouseWheel(WPARAM wState, int x, int y)
{
	musicScroll.OnMouseWheel(wState, x, y);
}
