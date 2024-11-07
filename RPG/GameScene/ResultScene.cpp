#include "framework.h"
#include "ResultScene.h"

ResultScene::ResultScene()
{
	text.SetLayout(L"Result");
}

ResultScene::~ResultScene()
{
}

void ResultScene::BeginScene()
{
	timer.Reset();
}

void ResultScene::OnResize(float newW, float newH)
{
}

void ResultScene::Update(float dt)
{
	timer.Tick();
	if (KEYBOARD.Down(VK_ESCAPE)) SCENEMANAGER.ChangeScene(SceneManager::Name::SelectMusic);
}

void ResultScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	text.Draw();
}

void ResultScene::EndScene()
{
	SCENEMANAGER.RemoveScene(SceneManager::Name::ResultScene);
}
