#include "framework.h"
#include "TestScene.h"

TestScene::TestScene() : sq(0.0f, 0.0f, 640.0f, 480.0f), sq2(300, 100, 259, 224)
{
	sq.SetTexture(GETTEXTURE("test"));
	sq2.SetTexture(GETTEXTURE("myon"));
}

TestScene::~TestScene()
{
}

void TestScene::BeginScene(float newW, float newH)
{
}

void TestScene::OnResize(float newW, float newH)
{
}

void TestScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE))
	{
		string* tempStr = new string(SceneManager::Name::Intro);
		SceneManager::GetInstance().ChangeScene(*tempStr);
		delete tempStr;
	}
	if (KEYBOARD.Press('Z'))
	{
		sq2.Position.x -= 400.0f * dt;
		sq2.UpdateWorld();
	}
	if (KEYBOARD.Press('X'))
	{
		sq2.Position.x += 400.0f * dt;
		sq2.UpdateWorld();
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