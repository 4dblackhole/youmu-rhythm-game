#include "framework.h"
#include "SelectMusicScene.h"

constexpr int MusicScrollX = -10;
constexpr int MusicScrollY = -10;
constexpr float MusicScrollWidth = 450.0f;
constexpr float MusicScrollHeight = 670.0f;

SelectMusicScene::SelectMusicScene() :
	background(0, 0, (float)StandardWidth, (float)StandardHeight, MyColor4::Black, true),
	musicScroll(MusicScrollX - MusicScrollWidth*0.5f, MusicScrollY - MusicScrollHeight*0.5f, MusicScrollWidth, MusicScrollHeight)
{
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
}

void SelectMusicScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE))
	{
		string tempStr(SceneManager::Name::Title);
		SceneManager::GetInstance().ChangeScene(tempStr);
	}
	musicScroll.Update(dt);

	if (KEYBOARD.Down(VK_RETURN))
	{
		const Pattern* p = musicScroll.GetCurrentPattern();
	}
}

void SelectMusicScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	background.Render(deviceContext, cam);
	musicScroll.Render(deviceContext, cam);
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
