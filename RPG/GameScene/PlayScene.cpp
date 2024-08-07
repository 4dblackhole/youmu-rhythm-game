#include "framework.h"
#include "PlayScene.h"

PlayScene::PlayScene() : youmu(300, 100, 259, 224)
{
	youmu.SetTexture(GETTEXTURE(TextureManager::Name::myon));
}

PlayScene::~PlayScene()
{
}

void PlayScene::BeginScene()
{
}

void PlayScene::OnResize(float newW, float newH)
{

}

void PlayScene::Update(float dt)
{
	if (KEYBOARD.Down(VK_ESCAPE)) SCENEMANAGER.ChangeScene(SceneManager::Name::SelectMusic);
}

void PlayScene::Render(ID3D11DeviceContext* deviceContext, const Camera& cam)
{
	youmu.Render(deviceContext, cam);
}

void PlayScene::EndScene()
{
	SCENEMANAGER.RemoveScene(SceneManager::Name::PlayScene);
}
