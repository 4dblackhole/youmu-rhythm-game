#include "framework.h"
#include "SelectMusicScene.h"

constexpr int MusicScrollX = -10;
constexpr int MusicScrollY = -10;
constexpr float MusicScrollWidth = 450.0f;
constexpr float MusicScrollHeight = 670.0f;

constexpr float NoMusicTextSize = 40.0f;
constexpr int NoMusicTextX = MusicScrollX - (int)MusicScrollWidth;
constexpr int NoMusicTextY = MusicScrollY + 80;

SelectMusicScene::SelectMusicScene() :
	background(0, 0, (float)0, (float)StandardHeight, MyColor4::Black, true),
	musicScroll(MusicScrollX, MusicScrollY, MusicScrollWidth, MusicScrollHeight)
{
	musicScroll.SetAlignX(AlignModeX::Right);
	musicScroll.SetAlignY(AlignModeY::Top);
	musicScroll.SetTexture(GETTEXTURE(TextureManager::Name::MusicScroll));

	DWRITE_TEXT_METRICS mt;
	LPCWSTR tempstr = L"No Music";
	IDWriteTextFormat*& tempFormat = D2D.GetFont(D2Ddevice::FontName::DefaultFont);

	DwLayout::GetLayoutMetrics(tempstr, tempFormat, &mt);

	LayoutDesc tempDesc(NoMusicTextSize, MyColorF::GhostGreen,
		{ NoMusicTextX + (int)(mt.width * NoMusicTextSize / D2Ddevice::DefaultFontSize),NoMusicTextY });

	tempDesc.maxW = MusicScrollWidth;
	tempDesc.alignX = AlignModeX::Right;
	musicScrollNoMusicText.reset(new DwLayout(tempDesc));
	musicScrollNoMusicText->SetLayoutRightAlign(tempstr, tempFormat);
	musicScrollNoMusicText->layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
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
	musicScroll.OnResize();
	musicScrollNoMusicText->Resize(newW, newH);
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
	musicScroll.Render(deviceContext, cam);
	if (musicCount == 0) musicScrollNoMusicText->Draw();
}

void SelectMusicScene::EndScene()
{
}
