#include "framework.h"
#include "TestScene.h"

TestScene::TestScene() : sq(0.0f, 0.0f, 1280.0f, 720.0f), sq2(300, 100, 259, 224), whiteSquare(300, -100, 259, 224,MyColor4::White,true)
{
	sq.SetTexture(GETTEXTURE(TextureManager::Name::test));
	constexpr float rate = (640.0 / 480.0) / (1280.0 / 720.0);
	sq.GetWorld3d().SetUvScale({ 1.0f, rate });
	sq.GetWorld3d().SetUvPosition({ 1.0f, rate });
	sq2.SetTexture(GETTEXTURE(TextureManager::Name::myon));
}

TestScene::~TestScene()
{
}

void TestScene::BeginScene()
{
}

void TestScene::OnResize(float newW, float newH)
{
	sq.ChangeWidthToCurrentWidth(newW, newH);
}

void TestScene::Update(float dt)
{
	WhiteSquareLeftRightMove(dt);

	if (KEYBOARD.Down(VK_ESCAPE))
	{
		string* tempStr = new string(SceneManager::Name::Title);
		SceneManager::GetInstance().ChangeScene(*tempStr);
		delete tempStr;
	}
	if (KEYBOARD.Hold('Z'))
	{
		sq2.GetWorld3d().MoveLocalPosition(-400.0f * dt, 0.0f, 0.0f);
	}
	if (KEYBOARD.Hold('X'))
	{
		sq2.GetWorld3d().MoveLocalPosition(400.0f * dt, 0.0f, 0.0f);
	}
}

void TestScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	sq.Render(deviceContext, cam);
	sq2.Render(deviceContext, cam);
	whiteSquare.Render(deviceContext, cam);
}

void TestScene::EndScene()
{
}

void TestScene::WhiteSquareLeftRightMove(float dt)
{
	static bool directionRight = false;
	float resultX = whiteSquare.GetWorld3d().GetLocalPosition().x;
	constexpr float speed = 2000.0f;
	if (directionRight) resultX += dt * speed;
	else				resultX -= dt * speed;

	const float orthoWidth = ShortCut::GetOrthoWidth((float)App->GetWidth(), (float)App->GetHeight());

	if (resultX <= -orthoWidth * 0.5f)
	{
		directionRight = true;
		resultX = -orthoWidth - resultX;
	}
	if (resultX >= orthoWidth * 0.5f)
	{
		directionRight = false;
		resultX = orthoWidth - resultX;
	}

	if (directionRight)
		whiteSquare.GetWorld3d().SetLocalPosition({ resultX, whiteSquare.GetWorld3d().GetLocalPosition().y, 0 });
	else
		whiteSquare.GetWorld3d().SetLocalPosition({ resultX, whiteSquare.GetWorld3d().GetLocalPosition().y, 0 });

}
