#include "framework.h"
#include "SelectMusicScene.h"

constexpr int MusicScrollX = -10;
constexpr int MusicScrollY = -10;
constexpr float MusicScrollWidth = 450.0f;
constexpr float MusicScrollHeight = 670.0f;

SelectMusicScene::SelectMusicScene() :
	background(0, 0, (float)0, (float)StandardHeight, MyColor4::Black, true),
	musicScroll(MusicScrollX, MusicScrollY, MusicScrollWidth, MusicScrollHeight)
{
}

SelectMusicScene::~SelectMusicScene()
{
}

void SelectMusicScene::BeginScene()
{
	musicScroll.PlayMusic();
}

void SelectMusicScene::OnResize(float newW, float newH)
{
	background.ChangeWidthToCurrentWidth(newW, newH);
	musicScroll.OnResize(newW, newH);
}

void SelectMusicScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE))
	{
		string tempStr(SceneManager::Name::Title);
		SceneManager::GetInstance().ChangeScene(tempStr);
	}
	musicScroll.Update(dt);
}

void SelectMusicScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	background.Render(deviceContext, cam);
	musicScroll.Render(deviceContext, cam);
}

void SelectMusicScene::EndScene()
{
	musicScroll.StopMusic();
}

void SelectMusicScene::OnMouseWheel(WPARAM wState, int x, int y)
{
	musicScroll.OnMouseWheel(wState, x, y);
}
