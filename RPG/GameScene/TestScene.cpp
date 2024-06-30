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

void TestScene::BeginScene()
{
}

void TestScene::OnResize(float newW, float newH)
{
}

void TestScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE))
	{
		SCENEMANAGER.ChangeScene(SceneManager::Name::Intro);
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
