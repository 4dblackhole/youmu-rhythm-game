#include "framework.h"

#include "BackGroundScene.h"

BackGroundScene::BackGroundScene() : bg(0.0f, 0.0f, 1280.0f, 960.0f)
{
}

BackGroundScene::~BackGroundScene()
{
}

void BackGroundScene::Init()
{
	bg.SetTexture(GETTEXTURE("test"));
}

void BackGroundScene::OnResize()
{
}

void BackGroundScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_F1)) SCENEMANAGER.ChangeScene("test");
}

void BackGroundScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	bg.Render(deviceContext, cam);
}

void BackGroundScene::Release()
{
}
