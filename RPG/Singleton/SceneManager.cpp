#pragma once
#include "framework.h"

#include "SceneManager.h"

#include "GameScene/LogoScene.h"
#include "GameScene/TestScene.h"

void SceneManager::InitScene(const mapSceneList::key_type& sceneName, const mapSceneList::mapped_type& scene)
{
	if (AddScene(sceneName, scene))
	{
		currentScene = scene;
		currentScene->BeginScene();
	}
}

bool SceneManager::AddScene(const mapSceneList::key_type& sceneName, const mapSceneList::mapped_type& scene)
{
	if (sceneList.find(sceneName) != sceneList.end())
	{
		delete scene;
		return false;
	}
	sceneList.insert(make_pair(sceneName, scene));
	return true;
}

bool SceneManager::RemoveScene(const mapSceneList::key_type& sceneName)
{
	mapSceneIter it = sceneList.find(sceneName);
	if (it != sceneList.end())
	{
		delete it->second;
		sceneList.erase(it);
		return true;
	}
	//sceneName not found
	return false;
}

void SceneManager::ChangeScene(const mapSceneList::key_type& sceneName)
{
	mapSceneIter it = sceneList.find(sceneName);
	if (it != sceneList.end())
	{
		currentScene->EndScene();
		currentScene = it->second;
		currentScene->BeginScene();
		currentScene->OnResize((float)App->GetWidth(), (float)App->GetHeight());
	}
}

SceneManager::SceneManager()
{
	GameScene* scene = new LogoScene;
	InitScene(SceneManager::Name::Logo, move(scene));

	scene = new TestScene;
	AddScene(SceneManager::Name::test, move(scene));
}

SceneManager::~SceneManager()
{
	currentScene->EndScene();
	for (const mapSceneList::value_type& it : sceneList)
	{
		delete it.second;
	}

	TRACE(_T("SceneManager Release\n"));
}
