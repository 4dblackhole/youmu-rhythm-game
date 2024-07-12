#include "framework.h"
#include "TestScene.h"

TestScene::TestScene() : sq(0.0f, 0.0f, 1280.0f, 720.0f), sq2(300, 100, 259, 224)
{
	sq.SetTexture(GETTEXTURE("test"));
	constexpr float rate = (640.0 / 480.0) / (1280.0 / 720.0);
	sq.SetUvScale({ 1.0f, rate });
	sq.SetUvPosition({ 1.0f, rate });
	sq2.SetTexture(GETTEXTURE("myon"));
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
	if (KEYBOARD.Down(VK_ESCAPE))
	{
		string* tempStr = new string(SceneManager::Name::Title);
		SceneManager::GetInstance().ChangeScene(*tempStr);
		delete tempStr;
	}
	if (KEYBOARD.Press('Z'))
	{
		sq2.ChangePosition(-400.0f * dt, 0.0f, 0.0f);
	}
	if (KEYBOARD.Press('X'))
	{
		sq2.ChangePosition(400.0f * dt, 0.0f, 0.0f);
	}
}

void TestScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	sq.Render(deviceContext, cam);
	sq2.Render(deviceContext, cam);
}

void TestScene::EndScene()
{
}