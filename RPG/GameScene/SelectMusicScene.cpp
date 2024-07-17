#include "framework.h"
#include "SelectMusicScene.h"

constexpr int MusicScrollX = -10;
constexpr int MusicScrollY = -10;
constexpr float MusicScrollWidth = 450.0f;
constexpr float MusicScrollHeight = 670.0f;

constexpr float NoMusicTextSize = 40.0f;
constexpr int NoMusicTextX = MusicScrollX - (int)MusicScrollWidth;
constexpr int NoMusicTextY = -MusicScrollY + 60; // each Y axis of 3D and 2D is reverse

constexpr int BoxHeight = 60;
constexpr int BoxOffsetY= -(int)MusicScrollY + 60;
constexpr int BoxEdgeX = 5;
constexpr int BoxEdgeY = 5;

SelectMusicScene::SelectMusicScene() :background(0, 0, (float)0, (float)StandardHeight, MyColor4::Black, true)
{
}

SelectMusicScene::~SelectMusicScene()
{
}

void SelectMusicScene::BeginScene()
{
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
}

void SelectMusicScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	background.Render(deviceContext, cam);
	//D2D.GetRenderTarget()->PushAxisAlignedClip({ 700,0,1200,300 }, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	musicScroll.Render(deviceContext, cam);
	//D2D.GetRenderTarget()->PopAxisAlignedClip();
}

void SelectMusicScene::EndScene()
{
}
