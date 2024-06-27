#pragma once
#include "System/GameScene.h"
#include "SceneManager.h"

void SceneManager::InitScene(const mapSceneList::key_type& sceneName, const mapSceneList::mapped_type& scene)
{
	AddScene(sceneName, scene);
	currentScene = scene;
	currentScene->Init();
}

void SceneManager::AddScene(const mapSceneList::key_type& sceneName, const mapSceneList::mapped_type& scene)
{
	assert(sceneList.find(sceneName) == sceneList.end());
	sceneList.insert(make_pair(sceneName, scene));
}

void SceneManager::ChangeScene(const mapSceneList::key_type& sceneName)
{
	mapSceneIter it = sceneList.find(sceneName);
	if (it != sceneList.end())
	{
		currentScene = it->second;
		currentScene->Init();
	}
}

SceneManager::~SceneManager()
{
	for (const mapSceneList::value_type&it : sceneList)	delete it.second;
}
